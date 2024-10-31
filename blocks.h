#ifndef BLOCKS_H
#define BLOCKS_H

#define BLOCK_SIZE 512

typedef struct {
    int start_offset; //inicio del offset dentro del archivo real donde escriben todos los archivos
    int end_offset;   //limite de escritura 
    int block_id;     //este id se usa para indexar la lista de bloques de memoria
} block;

typedef struct block_node{
    int block_id;     //al referenciar el bloque de un archivo, este id coincide con el id del bloque en memoria para busquedas eficientes
    struct block_node *prev;
    struct block_node *next;
} block_node;

typedef struct {
    block_node *head;
    block_node *tail;
    int size;         //tamano en bloques y no en bytes
} block_allocated_list;

typedef struct {
    block *memory_blocks; 
    bool *usage_registry;  //registro de bloques libres
    int size;
    char *name;           //nombre del storage, por ejemplo "C:"
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
void remove_node_at_end(block_allocated_list *allocated_blocks);
block_node *replace_block_in_node(block_allocated_list *allocated_blocks, int old_block_id, int new_block_id);

// Función para añadir un nodo a la lista de bloques asignados a un archivo
// Entrada: Puntero a block_allocated_list, ID del bloque (block_id)
// Salida: Ninguna. La función modifica directamente la lista de bloques asignados.
void add_node(block_allocated_list *allocated_blocks, int block_id) {
    block_node *new_block_node = (block_node*)malloc(sizeof(block_node));

    if(new_block_node==NULL){
        printf("Error: no se pudo anadir el bloque de memoria con id: %d\n", block_id);
        return;
    } else {
        // Inicializa el nuevo nodo
        new_block_node->block_id = block_id;
        new_block_node->prev = NULL;
        new_block_node->next = NULL;
        // Si la lista está vacía, se inicializa el nodo como cabeza y cola
        if(allocated_blocks->head == NULL){
            allocated_blocks->head = new_block_node;
            allocated_blocks->tail = new_block_node;
        } else {
            // Conecta el nuevo nodo al final de la lista
            block_node *temp = allocated_blocks->tail;
            temp->next = new_block_node;
            new_block_node->prev = temp;
            allocated_blocks->tail = new_block_node;
        }

        allocated_blocks->size++;
    }
}
// Función para eliminar un nodo al final de la lista de bloques asignados
// Entrada: Puntero a block_allocated_list
// Salida: Ninguna. La función modifica directamente la lista de bloques asignados.
void remove_node_at_end(block_allocated_list *allocated_blocks) {
    if(allocated_blocks->head==NULL){
        printf("No existe ningun bloque, cancelando operacion de remover al final\n");
        return;
    }
     // Caso especial: solo hay un nodo en la lista
    if (allocated_blocks->head == allocated_blocks->tail){
        free(allocated_blocks->head);
        allocated_blocks->head = allocated_blocks->tail = NULL; 
        allocated_blocks->size--;
        return;
    }
    // Elimina el nodo al final de la lista
    block_node *temp = allocated_blocks->tail;
    allocated_blocks->tail->prev->next = NULL;
    allocated_blocks->tail = allocated_blocks->tail->prev;
    allocated_blocks->size--;
    temp->prev = NULL;
    free(temp);
}
// Función para reemplazar un bloque en la lista de bloques asignados
// Entrada: Puntero a block_allocated_list, ID del bloque antiguo (old_block_id), ID del nuevo bloque (new_block_id)
// Salida: Puntero al nodo modificado, o NULL si ocurre algún error.
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
// Función para obtener un nodo en una posición específica de la lista de bloques asignados
// Entrada: Puntero a block_allocated_list, posición del nodo en la lista (position)
// Salida: Puntero al nodo en la posición especificada, o NULL si hay un error.
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
// Función para extender la lista de bloques asignados si hay suficiente espacio libre
// Entrada: Puntero a storage, puntero a block_allocated_list, cantidad de bloques a añadir (amount)
// Salida: True si se realizó la extensión correctamente, False en caso contrario.
bool extend_block_allocated_list(storage *file_system,  block_allocated_list * file_blocks, int amount) {
    int c = 0;
    int prev_size = file_blocks->size;
    if (amount == 0) {
        return true;
    }
    block *blocks_assigned = probe_free_blocks(file_system, amount, &c);
    if(blocks_assigned ==NULL) {
        return false;
    }

    if (c != amount) {
        return false;  
    }
    // Añade los nuevos bloques a la lista de bloques asignados
    for (int i = 0; i < amount; i++) {
        add_node(file_blocks, blocks_assigned[i].block_id);  // Add nodes to the file_blocks list
    }

    if(file_blocks->size!=prev_size+amount) {
        return false;
    }
    // Actualiza el registro de uso en el sistema de archivos
    for (int i = 0; i < amount; i++) {
        file_system->usage_registry[blocks_assigned[i].block_id]=true;  // Add nodes to the file_blocks list
    }

    return true;

}
// Convierte un array de enteros en una cadena con formato para su impresión
// Entrada: Array de enteros (array), tamaño del array (size), cadena de resultado (result)
// Salida: Ninguna. La cadena de resultado se almacena en "result".
void int_array_to_string(int *array, int size, char *result) {
    int offset = 0;

    if(size<=0){
        sprintf(result, "%s", "Vacio\n");
        return;
    }

    for (int i = 0; i < size; i++) {
        offset += sprintf(result + offset, "%d", array[i]);
        
       // Añade un separador entre los elementos, excepto después del último
        if (i < size - 1) {
            offset += sprintf(result + offset, " <--> ");
        }
    }

    result[offset] = '\0';
}
// Función para obtener una cadena con la lista de IDs de bloques asignados
// Entrada: Puntero a block_allocated_list
// Salida: Puntero a cadena con la lista de IDs de bloques, o NULL si hay un error.
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
            printf("Mismatch en el tamano de lista de bloques o lista corrupta\n");
            return NULL;
        }
        block_ids[i] = temp->block_id;
        temp = temp->next;
    }
    // Asigna memoria para la cadena y convierte los IDs de bloques a una cadena formateada
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
// Crea bloques de memoria con identificadores asociados a un bloque real en memoria
// y offsets definidos
// Entrada: Cantidad de bloques a crear (amount)
// Salida: Puntero al array de bloques creados, o NULL si hay un error.
block *create_blocks(int amount){
    block *mem_blocks= (block*)malloc(sizeof(block) * amount);

    if(mem_blocks == NULL){
        printf("No se pudo asignar memoria para crear los bloques de memoria del sistema\n");
        return NULL;
    }

    int offset_start = 0;
    int offset_end = 511;
    // Inicializa cada bloque con su offset y un identificador de bloque asociado
    for (int i = 0; i < amount; i++){

        block new_block = {offset_start, offset_end, i};
        mem_blocks[i] = new_block;
        offset_start = offset_end +1;
        offset_end = offset_end + 512;
    }

   return mem_blocks; 
}
// Busca y asigna bloques libres en el sistema de archivos
// Entrada: Puntero a storage, cantidad de bloques necesarios (amount), puntero a contador de bloques asignados (*c)
// Salida: Puntero a los bloques asignados, o NULL si no se encontraron suficientes bloques.
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
        //printf("Uso de registro en indice: %d es : %d - %d \n", i, file_system->usage_registry[i], file_system->bad_blocks[i]);
        if (file_system->usage_registry[i] == 0) {
            free_blocks_taken[k] = file_system->memory_blocks[i];
            (*c)++;  
            k++;
        }
    }

    if (*c != amount) {
        printf("No se lograron obtener los bloques solicitados, se encontraron: %d bloques libres\n", *c);
        free(free_blocks_taken);
        return NULL;
    }

    return free_blocks_taken;
}
// Inicializa el sistema de almacenamiento, incluyendo la memoria para los bloques y su registro de uso
// Entrada: Puntero a storage, tamaño del sistema (size), nombre del sistema de archivos (name)
// Salida: True si la inicialización fue exitosa, False en caso contrario.
bool initialize_storage(storage *file_system, int size, char *name) {
    bool *usage_registry = (bool*)calloc(sizeof(bool), size);

    if(usage_registry == NULL){
        perror("Error: no se logro inicializar el sistema de archivos\n");
        return false;
    }

    block *mem_blocks = create_blocks(size);

    if(mem_blocks == NULL) {
        perror("Error: no se logro inicializar el sistema de archivos\n");
        free(usage_registry);
        return false;
    }

    file_system->usage_registry = usage_registry;
    file_system->memory_blocks= mem_blocks;
    file_system->size = size;
    file_system->name = name;
    return true;
}
// Limpia la memoria asignada al sistema de almacenamiento
// Entrada: Puntero a storage
// Salida: Ninguna. La función libera la memoria asignada.
void clean_up_storage(storage *storage) {
    free(storage->usage_registry);
    free(storage->memory_blocks);
    free(storage);
}

#endif // !BLOCKS_H
