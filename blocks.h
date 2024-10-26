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
    bool *usage_registry;
    bool *bad_blocks;
    int size;
    char *name;
} storage;

void add_node(block_allocated_list *allocated_blocks, int block_id);
block *create_blocks(int amount);
void clean_up_storage(storage *storage);
bool extend_block_allocated_list(storage *file_system, block_allocated_list *file_blocks, int amount);
block_node *get_node_at_position(block_allocated_list *allocated_blocks, int position);
void int_array_to_string(int *array, int size, char *result);
bool initialize_storage(storage *file_system, int size, char *name);
block *probe_free_blocks(storage *file_system, int amount, int *c);
char *print_block_list(block_allocated_list *block_list);
block_node *remove_node_at_end(block_allocated_list *allocated_blocks);
block_node *replace_block_in_node(block_allocated_list *allocated_blocks, int old_block_id, int new_block_id);


void add_node(block_allocated_list *allocated_blocks, int block_id) {
    block_node *new_block_node = (block_node*)malloc(sizeof(block_node));

    if(new_block_node==NULL){
        printf("Error: no se pudo anadir el bloque de memoria con id: %d", block_id);
        return;
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
    if (allocated_blocks->head == NULL){
        printf("Error: accediendo a lista de bloques sin inicializar\n");
        return NULL;
    }
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

block_node *get_node_at_position(block_allocated_list * allocated_blocks, int position) {
    if (allocated_blocks->head == NULL){
        printf("Error: accediendo a lista de bloques sin inicializar\n");
        return NULL;
    }
    if (position < 0 || position >= allocated_blocks->size){
        printf("Error: posicion invalida para bloque\n");
        return NULL;
    }

    block_node *temp = allocated_blocks->head;
    int i = 0;
    while(i != position) {
        temp = temp->next;

        if(temp == NULL) {
            printf("Error: se alcanzo una posicion nula\n");
            return NULL;
        }
        i++;
    }

    return temp;

}

bool extend_block_allocated_list(storage *file_system,  block_allocated_list * file_blocks, int amount) {
    int c = 0;
    block *blocks_assigned = probe_free_blocks(file_system, amount, &c);
    if(blocks_assigned ==NULL) {
        printf("No se lograron asignar los bloques solicitads\n");
        return false;
    }

    if (c != amount) {
        return false;  
    }

    for (int i = 0; i < amount; i++) {
        add_node(file_blocks, blocks_assigned[i].block_id);  // Add nodes to the file_blocks list
    }

    if(file_blocks->size!=amount) {
        printf("No se lograron asignar los bloques solicitads\n");
        return false;
    }

    for (int i = 0; i < amount; i++) {
        file_system->usage_registry[blocks_assigned[i].block_id]=true;  // Add nodes to the file_blocks list
    }

    return true;

}

void int_array_to_string(int *array, int size, char *result) {
    int offset = 0;

    if(size<=0){
        sprintf(result, "%s", "empty array\n");
        return;
    }

    for (int i = 0; i < size; i++) {
        offset += sprintf(result + offset, "%d", array[i]);
        // Convert the integer to string and store it in result
        
        // Add a comma and space after every integer except the last one
        if (i < size - 1) {
            offset += sprintf(result + offset, " <--> ");
        }
    }

    result[offset] = '\0';
}

char *print_block_list(block_allocated_list *block_list){
    if(block_list->size<=0){
        char *empty_message = (char *)malloc(13);
        if (empty_message==NULL){
            printf("Fallo de asignacion de memoria para imprimir error\n");
            return NULL;
        } 
        strcpy(empty_message, "Array vacio\n");
        return empty_message;
    }

    int *block_ids = (int *)malloc(sizeof(int) * block_list->size); 
    if (block_ids == NULL) {
        printf("Fallo de asginacion de memoria para crear string de bloques asignados\n");
        return NULL;
    }

    block_node *temp = block_list->head;
    for(int i = 0; i < block_list->size; i++){
        if (temp == NULL) {
            free(block_ids);
            printf("Mismatch en el tamano de lista de bloques o lista corrupta");
            return NULL;
        }
        block_ids[i] = temp->block_id;
        temp = temp->next;
    }

    char *string = (char*)malloc(block_list->size * (10 +6) +1);
    if (string == NULL) {
        printf("Error asignando memoria para imprimir string\n");
        free(block_ids);
        return NULL;
    }

    int_array_to_string(block_ids, block_list->size, string);
    free(block_ids);
    return string;
}

// offset start, offset end, id, checksum
block *create_blocks(int amount){
    block *mem_blocks= (block*)malloc(sizeof(block) * amount);

    if(mem_blocks == NULL){
        printf("No se pudo asignar memoria para crear los bloques de memoria del sistema");
        return NULL;
    }

    int offset_start = 0;
    int offset_end = 511;
    int i = 0;

    for (i = 0; i < amount; i++){

        printf("bloque: %d, offset start: %d, offset_end: %d\n", i, offset_start, offset_end);
        block new_block = {offset_start, offset_end, i, 1};
        mem_blocks[i] = new_block;
        offset_start = offset_end +1;
        offset_end = offset_end + 512;
    }

   return mem_blocks; 
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
        return NULL;
    }

    return free_blocks_taken;
}

//temporalmente forzando los bloques como atributo, en versiones posteriores se inicializaria todo desde aca
bool initialize_storage(storage *file_system, int size, char *name) {
   
    bool *bad_blocks = (bool*)calloc(sizeof(bool),size);
    bool *usage_registry = (bool*)calloc(sizeof(bool), size);

    if(bad_blocks==NULL || usage_registry == NULL){
        perror("Error: no se logro inicializar el sistema de archivos\n");
        return false;
    }

    block *mem_blocks = create_blocks(size);

    if(mem_blocks == NULL) {
        perror("Error: no se logro inicializar el sistema de archivos\n");
        return false;
    }

    file_system->bad_blocks = bad_blocks;
    file_system->usage_registry = usage_registry;
    file_system->memory_blocks= mem_blocks;
    file_system->size = size;
    file_system->name = name;
    return true;
}

void clean_up_storage(storage *storage) {
    free(storage->bad_blocks);
    free(storage->usage_registry);
    free(storage->memory_blocks);
    free(storage);
}

#endif // !BLOCKS_H
