#ifndef BLOCKS_H
#define BLOCKS_H

#define BLOCK_SIZE 512

typedef struct {
    int start_offset;
    int end_offset;
    int block_id;
    int checksum;
} block;

typedef struct block_node{
    int block_id;
    struct block_node *prev;
    struct block_node *next;
} block_node;

typedef struct {
    block_node *head;
    block_node *tail;
    int size;
} block_allocated_list;

typedef struct {
    block *memory_blocks; 
    int *usage_registry;
    bool *bad_blocks;
    int size;
} storage;

void add_node(block_allocated_list *allocated_blocks, int block_id);
block_node *remove_node_at_end(block_allocated_list *allocated_blocks);
block_node *replace_block_in_node(block_allocated_list *allocated_blocks, int id_block, int id_block_new);
void clean_up_storage(storage *storage);
void int_array_to_string(int *array, int size, char *result);
char *print_block_list(block_allocated_list *block_list);

void add_node(block_allocated_list *allocated_blocks, int block_id) {
    block_node *new_block_node = (block_node*)malloc(sizeof(block_node));

    if(new_block_node==NULL){
        printf("Error: no se pudo anadir el bloque de memoria con id: %d", block_id);
    } else {
        new_block_node->block_id = block_id;
        new_block_node->prev = NULL;
        new_block_node->next = NULL;

        if(allocated_blocks->head == NULL){
            allocated_blocks->head = new_block_node;
            allocated_blocks->tail = new_block_node;
        } else {
            block_node *temp = allocated_blocks->tail;
            temp->next = new_block_node;
            new_block_node->prev = temp;
            allocated_blocks->tail = new_block_node;
        }

        allocated_blocks->size++;
    }
}

block_node *remove_node_at_end(block_allocated_list *allocated_blocks) {
    if(allocated_blocks->head==NULL){
        printf("No existe ningun bloque, cancelando operacion de remover al final\n");
        return NULL;
    }

    if (allocated_blocks->head == allocated_blocks->tail){
        printf("No se permite borrar el ultimo bloque de una lista si este es el unico asignado\n");
        return NULL;
    }

    block_node *temp = allocated_blocks->tail;
    allocated_blocks->tail->prev->next = NULL;
    allocated_blocks->tail = allocated_blocks->tail->prev;
    allocated_blocks->size--;
    temp->prev = NULL;
    return temp;

}

block_node *replace_block_in_node(block_allocated_list *allocated_blocks, int old_block_id, int new_block_id) {
    if (old_block_id < 0 || new_block_id <0) {
        printf("Id de bloque invalida para lista de bloques asignados");
        return NULL;
    }

    block_node *temp = allocated_blocks->head;
    while(temp!=NULL){
        if (temp->block_id == old_block_id) {
            temp->block_id = new_block_id;
            return temp;
        }
        temp = temp->next;
    }
    printf("Id de bloque invalida para lista de bloques asignados");
    return NULL;
}

void int_array_to_string(int *array, int size, char *result) {
    int offset = 0;

    if(size<=0){
        sprintf(result, "%s", "empty array\n");
    }

    for (int i = 0; i < size; i++) {
        offset += sprintf(result + offset, "%d", array[i]);
        // Convert the integer to string and store it in result
        
        // Add a comma and space after every integer except the last one
        if (i < size - 1) {
            offset += sprintf(result + offset, " <--> ");
        }
    }
}

char *print_block_list(block_allocated_list *block_list){

    int i;
    int *block_ids;
    block_ids = (int *)malloc(sizeof(int) * block_list->size); 
    block_node *temp = block_list->head;
    char *string; 
    string = (char*)malloc(block_list->size * (10 +6));
    for(int i = 0; i < block_list->size; i++){
        block_ids[i] = temp->block_id;
        temp = temp->next;
    }
    int_array_to_string(block_ids, block_list->size, string);

    return string;
}

void clean_up_storage(storage *storage) {
    free(storage->bad_blocks);
    free(storage->usage_registry);
    free(storage->memory_blocks);
    free(storage);
}

//temporalmente forzando los bloques como atributo, en versiones posteriores se inicializaria todo desde aca
bool initialize_storage(storage *file_system, int size, block * mem_blocks ) {
   
    bool *bad_blocks = (bool*)calloc(sizeof(bool),size);
    int *usage_registry = (int*)calloc(sizeof(int), size);
    block *uninitialized_blocks = (block*)malloc(sizeof(block)*size);

    if(bad_blocks==NULL || usage_registry == NULL || uninitialized_blocks == NULL){
        perror("Error: no se logro inicializar el sistema de archivos\n");
        return false;
    }
    uninitialized_blocks = mem_blocks;
    file_system->bad_blocks = bad_blocks;
    file_system->usage_registry = usage_registry;
    file_system->memory_blocks= uninitialized_blocks;
    file_system->size = size;
    return true;
}


block *probe_free_blocks(storage *file_system, int amount, int *c){
    int i;
    int k = 0;
    block *free_blocks_taken;
    free_blocks_taken = (block*)malloc(sizeof(block) * amount);

    if (free_blocks_taken == NULL) {
        printf("No se logro asignar memoria para los bloques\n");
        return NULL;
    }

    for (i = 0; i < file_system->size && k < amount; i++) {
        if (file_system->usage_registry[i] == 0 && file_system->bad_blocks[i] == 0) {
            free_blocks_taken[k] = file_system->memory_blocks[i];
            (*c)++;  
            k++;
        }
    }

    if (*c != amount) {
        printf("No se lograron obtener los bloques solicitados, se retornan: %d bloques libres\n", *c);
    }

    return free_blocks_taken;
}


#endif // !BLOCKS_H
