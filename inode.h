#ifndef INODE_H 
#define INODE_H

#define BINARY "bin"
#define TEXT "txt" 
#define DIRECTORY "dir" 
#define FILESYSTEM "fsystem"
#define BLOCK_SIZE 512
#define SAMPLE_DATE "10/23/2024"
#define USER "usuario123"

struct block_allocated_list;

typedef struct {
    int *checksums;
    bool modified;
    int size;
    char *created_by;
    char *last_modified;
    char *created_at;
    char *last_access;
    block_allocated_list * file_blocks;
} inode;  

typedef struct file{
    char *filepath;
    inode *inode;
    //struct file *files;     EXTRA: implementar logica de directorios
    bool bad_file;
} file;

/* Flag = 0 => no se ha usado el hash
 * Flag = 1 => se uso el hash previamente
 */ 
typedef struct {
    file *file_ref;
    int flag;
} hashable_item_file;

typedef struct {
    int size; 
    int current_occupied;
    hashable_item_file * table;
} hash_table_files;

bool create_file(int size, char *filepath, storage *file_system, hash_table_files *hash_t);
void clean_up_file(file *file, storage *storage, hash_table_files *hash_t);
file *find_file(char *filepath, hash_table_files *hash_t);
unsigned long hash1(const char *key, int hash_table_size);
unsigned long hash2(const char *key, int hash_table_size);
bool initialize_file(file *new_file, char *filepath, inode *inode, hash_table_files *hash_t);
bool initialize_file_blocks(int effective_size, storage *file_system, block_allocated_list *file_blocks);
bool initialize_file_system(storage *storage, char *name, int size);
bool initialize_inode(int effective_size, storage *file_system, inode *new_inode);
bool insert_into_hashtable(file *file, hash_table_files *hash_t);
void print_files(hash_table_files *hash_t);
void read_file(char *filepath, size_t offset, size_t amount, storage *file_system, hash_table_files *hash_t);
bool remove_from_hashtable(file *file, hash_table_files *hash_t);
void write_file(char *filepath, size_t offset, char *data, storage *file_system, hash_table_files *hash_t);


unsigned long  hash1 (const char *key, int hash_table_size){
    unsigned long hash = 5381;
    int c; 
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) +c;
    }
    return hash % hash_table_size; 
}

unsigned long hash2 (const char *key, int hash_table_size) {
    unsigned long hash = 2166136261U; 
    int c; 
    while ((c = *key++)){
        hash = (hash^c)* 16777619;
    }
    return (hash % (hash_table_size-1)) +1;
}

file *find_file(char *filepath, hash_table_files *hash_t) {
    unsigned long hash_1 = hash1(filepath, hash_t->size);
    unsigned long hash_2 = hash2(filepath, hash_t->size);
    hashable_item_file *hash_table = hash_t->table;
    int index = (int)hash_1;
    int c = 0;
    
    while (hash_table[index].flag == 1) {  // Check if an entry is presenta
        char *s = strdup(hash_table[index].file_ref->filepath);

        if(s==NULL){
            printf("Error de asignacion de memoria\n");
            return NULL;
        }

        if (strcmp(s, filepath) == 0) {
            //printf("Archivo: %s encontrado, retornado su referencia \n", filepath);
            free(s);
            return hash_table[index].file_ref;  // Return the found file reference
        }

        index = (int)((hash_1 + c * hash_2) % hash_t->size);
        c++;

        if (c >= hash_t->size) {
            break;
        }
    }

    printf("Archivo: %s no encontrado\n", filepath);
    return NULL;
}

bool insert_into_hashtable(file *file, hash_table_files *hash_t){
    unsigned long hash_1 = hash1(file->filepath, hash_t->size);
    unsigned long hash_2 = hash2(file->filepath, hash_t->size);
    hashable_item_file *hash_table = hash_t->table;
    int index = (int)hash_1;
    printf("Index is %d\n", index);

    int c = 0;

    while(hash_table[index].flag==1) {
        if (index==hash_1){
            printf("Fallo el insertado en tabla hash\n");
            return false;
        }
        if (c > hash_t->size) {
            printf("No existe mas espacio para achivos en la tabla\n");
            return false;
        } else {
            index = (hash_1 + c* hash_2) % hash_t->size;
            c++;
            printf("Index is %d\n", index);
            printf("C is %d\n", c);
        }
    }
    
    hash_table[index].file_ref= file;
    hash_table[index].flag = 1;
    hash_t->current_occupied++;
    return true;
}

bool remove_from_hashtable(file *file, hash_table_files *hash_t){
    unsigned long hash_1 = hash1(file->filepath, hash_t->size);
    unsigned long hash_2 = hash2(file->filepath, hash_t->size);
    hashable_item_file *hash_table = hash_t->table;
    int index = (int)hash_1;
    int c = 0;

    while(hash_table[index].flag==1) {
        if (strcmp(hash_table[index].file_ref->filepath, file->filepath)==0){
           printf("Archivo: %s encontrado, removiendo su referencia\n", file->filepath);
           hash_table[index].file_ref=NULL;
           hash_table[index].flag= 0;
           hash_t->current_occupied--;
           return false;
        }
        index = (hash_1 + c* hash_2) % hash_t->size;
        c++;
    }
    
    printf("Archivo: %s no encontrado,  no se pudo remover su referencia\n", file->filepath);
    return false;
}

// Main function to print files and their associated blocks
void print_files(hash_table_files *hash_t) {
    hashable_item_file *hash_table = hash_t->table;
    int i;
    printf("\n\n╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗");
    printf("\n║                                               Archivos del Sistema                                                         ║");
    printf("\n╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("╔═════╦═══════════════════════════════════════════╦═══════════════════════════════════════════════════════════════════════════╦═══════════════════╗\n");
    printf("║  Id ║ Nombre                                    ║ Bloques Asignados                                                         ║ Tamaño del archivo║\n");
    printf("╠═════╬═══════════════════════════════════════════╬═══════════════════════════════════════════════════════════════════════════╬═══════════════════╣\n");
    
    for (i = 0; i < hash_t->size; i++) {
        if (hash_table[i].flag == 1) {
            char *filepath = truncate_string(hash_table[i].file_ref->filepath, 40); // Limit filepath to 40 chars
            file *current_file = find_file(filepath, hash_t);
            block_allocated_list *printable_list = current_file->inode->file_blocks;
            char *block_l = print_block_list(printable_list); // Assuming this function returns a string with blocks
            
            // Truncate block list if too long (extended to 80 characters now)
            char *truncated_blocks = truncate_string(block_l, 80);
            
            // Print formatted row with alignment
            printf("║ %-3d ║ %-41s ║ %-73s ║ %-17d ║\n", 
                   i, 
                   filepath, 
                   truncated_blocks, 
                   current_file->inode->size);
            printf("╠═════╬═══════════════════════════════════════════╬═══════════════════════════════════════════════════════════════════════════╬═══════════════════╣\n");
            
            // Free allocated memory
            free(filepath);
            free(truncated_blocks);
            free(block_l);
        }
    }
    
    printf("╚═════╩═══════════════════════════════════════════╩═══════════════════════════════════════════════════════════════════════════╩═══════════════════╝\n");
}

//TODO cuando cmabie initialize storage, se hara la inicializacion de los bloques de memoria desde una funcion llamada internamente en vez de como parametro;
bool initialize_file_system(storage *storage, char *name, int size){
    //inicializar archivo para simulacion de filesystem;
    if(!create_and_prefill_file(name, size, ' ')){
            return false;
    };
    int effective_size = (int)floor(size/BLOCK_SIZE);
    printf("Tamano efectivo de escritura/lectura: %d bloques\n", effective_size);
    //inicializar el archivo
    if(!initialize_storage(storage, effective_size, name)){
        return false;
    }; 
    
    return true;
}


bool initialize_file_blocks(int effective_size, storage *file_system, block_allocated_list *file_blocks){
    file_blocks->tail = NULL;
    file_blocks->head = NULL;
    file_blocks->size = 0;


    return extend_block_allocated_list(file_system, file_blocks, effective_size);
}

bool initialize_inode(int size, storage *file_system, inode *new_inode) {
    int effective_size= (int)ceil((float)size/BLOCK_SIZE);
    printf("Effective size is: %d\n", effective_size);
    int i;
    block_allocated_list *file_blocks = (block_allocated_list*)malloc(sizeof(block_allocated_list));
    if(file_blocks==NULL) {
        printf("No se lograron asignar los bloques solicitads\n");
        return false;
    }

    if(!initialize_file_blocks(effective_size, file_system, file_blocks)) {
        printf("No se lograron asignar los bloques solicitads\n");
        return false;
    }

    int *checksums = (int *)calloc(effective_size, sizeof(int));
    if (checksums == NULL) {
        printf("Error: no se pudieron generar los checksums del archivo siendo creado\n");
        return false;
    }
    
    // Set inode attributes
    new_inode->checksums = checksums;
    new_inode->modified = false;
    new_inode->size = size;
    new_inode->created_by = USER;             
    new_inode->last_modified = SAMPLE_DATE;   
    new_inode->created_at = SAMPLE_DATE;      
    new_inode->last_access = SAMPLE_DATE;     
    new_inode->file_blocks = file_blocks;    

    return true;
}


bool initialize_file(file *new_file, char *filepath, inode *inode, hash_table_files *hash_t){
    
    new_file->filepath=filepath;
    new_file->inode=inode;
    new_file->bad_file=false;
    printf("Tamano convertido es en inode dentro del archivo es  %d\n", new_file->inode->size);


    if(!insert_into_hashtable(new_file, hash_t)){
        printf("Error generando archivo \n"); 
        return false;
    }

    return true;
}

bool create_file(int size, char* filepath, storage *file_system, hash_table_files *hash_t) {

    char *s = strndup(filepath,strlen(filepath));
    if (s == NULL) {
        printf("No se pudo asignar memoria para crear el archivo\n");
        return false;
    }

    inode *file_inode = (inode*)malloc(sizeof(inode));   
    if (file_inode == NULL) {
        printf("No se pudo asignar memoria para crear el archivo\n");
        return false;
    }

    if(!initialize_inode(size, file_system, file_inode)){
        printf("No se pudo asignar memoria para crear el archivo\n");
        return false;
    }

    file *new_file = (file*)malloc(sizeof(file));
    /*
    */

    if(!initialize_file(new_file,s,file_inode, hash_t)){
        printf("Archivo no pudo ser inicializado\n");
        return false;
    }
    printf("Archivo generado con exito\n");
    //todo: meterlo a la tabla de archivos
    return true;
}

void read_file(char *filepath, size_t offset, size_t amount, storage *file_system, hash_table_files *hash_t){
     file *file_read = find_file(filepath, hash_t);
     if (file_read==NULL){
         printf("Error, no se pudo encontrar el archivo\n");
         return;
     }
     if (file_read->inode->size < offset+amount){
         printf("Error, no se puede leer en una posicion fuera del limite del archivo\n");
         return;
     } 
     off_t starting_block_position = (int)floor(offset/BLOCK_SIZE);
     off_t starting_point_in_block =  offset - BLOCK_SIZE*starting_block_position;
     size_t batch_size = MIN(amount, MIN((BLOCK_SIZE - starting_point_in_block), (file_read->inode->size - offset)));
     size_t total_bytes_read = 0;
     block_node *block_node_read_currently = get_node_at_position(file_read->inode->file_blocks, starting_block_position);
     block *block_read_currently = &file_system->memory_blocks[block_node_read_currently->block_id];
     printf("Puntos importantes ahora son: %d para el bloque donde se inicia lectura y %d para offset de inicio desde ese bloque\n", starting_block_position, starting_point_in_block);
 
     if (block_read_currently == NULL) {
        return; 
     }
 
     char *buffer_content_read = (char*)malloc(sizeof(char)*amount + 1);
     if (buffer_content_read==NULL){
         printf("Error: no se pudo asignar memoria\n");
         return;
     }
     
     int fd = open(file_system->name, O_RDONLY);
     if(fd < 0){
         printf("Sistema de archivos corrupto, abortando \n");
         close(fd);
         abort();
     }
     
     printf("Empezando a leer el archivo: \n");
     while(block_node_read_currently!=NULL && (amount-total_bytes_read)!=0){
         //append_to_buffer(int fd, char *buffer, size_t buffer_size, size_t *current_pos, size_t read_size, off_t file_offset) 
         block_read_currently = &file_system->memory_blocks[block_node_read_currently->block_id];
         if(append_to_buffer(fd, buffer_content_read, (size_t) amount + 1, &total_bytes_read, batch_size, (off_t)block_read_currently->start_offset + starting_point_in_block) < 0){
             printf("Error leyendo el archivo, cancelando operacion");
             close(fd);
             return;
         } 
         batch_size = (amount - total_bytes_read >= BLOCK_SIZE) ? BLOCK_SIZE : (amount - total_bytes_read);
         starting_point_in_block = 0;
         block_node_read_currently = block_node_read_currently->next;
     }
 
     buffer_content_read[amount] = '\0';
     printf("\n Contenido: \t %s \n", buffer_content_read);
 
     close(fd);
}



bool extend_file(int offset, size_t data_size, file *file_written, storage*file_system){
    int surplus = (offset + data_size) - file_written->inode->size;

    int required_blocks = (int)ceil((float)(file_written->inode->size + surplus) / BLOCK_SIZE);
    int current_blocks = file_written->inode->file_blocks->size;
    if (surplus > 0 && required_blocks >= current_blocks) {
        // Extend the allocated blocks if necessary
        if (!extend_block_allocated_list(file_system, file_written->inode->file_blocks, required_blocks-current_blocks)) {
            printf("Error, no se pudo extender el archivo para escribir sobre él\n");
            return false;
        }
        printf("Se asignaron más bloques de memoria al archivo\n");
        file_written->inode->size += surplus;
    }

    return true;
}


//tener cuidado, al llamar a write en el parseo agragar el caracyer nulo en el buffer de data para que pueda ejecutar bien la funcion
void write_file(char *filepath, size_t offset, char *data, storage *file_system, hash_table_files *hash_t) {
    // Locate the file in the hash table
    file *file_written = find_file(filepath, hash_t);
    if (file_written == NULL) {
        printf("Error, no se pudo encontrar el archivo\n");
        return;
    }

    // Validate the write position
    if (file_written->inode->size < offset) {
        printf("Error, no se puede escribir en una posición fuera del límite del archivo\n");
        return;
    }

    // Determine the size of the data to be written
    size_t data_size = strlen(data);
    if (!extend_file(offset, data_size, file_written, file_system)) {
        return;
    }

    off_t starting_block_position = (int)floor((double)offset / BLOCK_SIZE);
    off_t starting_point_in_block = offset % BLOCK_SIZE;
    size_t batch_size = MIN(data_size, MIN((BLOCK_SIZE - starting_point_in_block), (file_written->inode->size - offset)));
    size_t total_bytes_written = 0;
    block_node *current_block_node = get_node_at_position(file_written->inode->file_blocks, starting_block_position);
    if (current_block_node == NULL) {
        printf("Error: Bloque inicial no encontrado\n");
        return;
    }

    block *current_block = &file_system->memory_blocks[current_block_node->block_id];
    if (current_block == NULL) {
        printf("Error: Bloque de memoria no encontrado\n");
        return;
    }

    int fd = open(file_system->name, O_WRONLY);
    if (fd < 0) {
        printf("Sistema de archivos corrupto, abortando\n");
        close(fd);
        abort();
    }

    // Write data to the file in batches
    while (current_block_node != NULL && (data_size - total_bytes_written) != 0) {
        // Update current block
        current_block = &file_system->memory_blocks[current_block_node->block_id];

        // Write data to the file from the buffer
        if (write_from_buffer(fd, data, data_size, &total_bytes_written, batch_size,
                              (off_t)current_block->start_offset + starting_point_in_block) < 0) {
            printf("Error escribiendo el archivo, cancelando operación\n");
            close(fd);
            return;
        }

        // Calculate the next batch size and update the offset within the block
        batch_size = MIN(data_size - total_bytes_written, BLOCK_SIZE);
        starting_point_in_block = 0; // After the first batch, the offset within the block is zero

        // Move to the next block in the list
        current_block_node = current_block_node->next;
    }

    // Close the file descriptor after writing
    close(fd);
}


void delete_file(char *filepath, storage *file_system, hash_table_files *hash_t){
    file *file_deleted = find_file(filepath, hash_t);
    if(!file_deleted){
       return;
    }

    clean_up_file(file_deleted, file_system, hash_t);
    if(!remove_from_hashtable(file_deleted, hash_t)){
        return;
    }

    free(file_deleted->filepath);
    free(file_deleted);
    printf("Archivo %d borrado con exito\n");
}

//void write_file(char *filepath, size_t offset, char *data, storage *file_system, hash_table_files *hash_t) {
void clean_up_file(file *file, storage *file_system, hash_table_files *hash_t){
    inode *file_inode = file->inode;
    block_allocated_list *file_blocks  = file_inode->file_blocks;
    block_node *temp = file_blocks->tail;
    char *overwrite = create_x_char_filled_array(file_inode->size, ' ');
    write_file(file->filepath, 0, overwrite, file_system, hash_t);
        
    while(file_blocks->size !=0 & temp!=NULL){
        file_system->usage_registry[temp->block_id] = false;
        temp = temp->prev;
        remove_node_at_end(file_blocks);
    }
    free(file_inode->file_blocks);
    free(file_inode->checksums);
    free(file_inode);
    /*
    free(file_inode->last_access);
    free(file_inode->created_at);
    free(file_inode->last_modified);
    free(file_inode->created_by);
    */
}











#endif // !DEBUG
