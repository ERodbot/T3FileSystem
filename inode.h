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

unsigned long hash1 (const char *key, int hash_table_size);
unsigned long hash2 (const char *key, int hash_table_size);
file *find_file(char *filepath, hash_table_files *hash_t);
bool insert_into_hashtable(file *file, hash_table_files *hash_t);
bool remove_from_hashtable(file *file, hash_table_files *hash_t);
void print_files(hash_table_files *hash_t);
void clean_up_file(file *file);

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
    int index = hash_1;
    int c = 0;

    while(hash_table[index].flag==1) {
        if (strcmp(hash_table[index].file_ref->filepath, file->filepath)==0){
           printf("Archivo: %s encontrado, removiendo su referencia\n", file->filepath);
           hash_table[index].file_ref=NULL;
           hash_table[index].flag= 0;
           hash_t->current_occupied--;
        }
        index = (hash_1 + c* hash_2) % hash_t->size;
        c++;
    }
    
    printf("Archivo: %s no encontrado,  no se pudo remover su referencia\n", file->filepath);
    return true;
}


// Helper function to truncate strings that are too long
char* truncate_string(const char* input, int max_len) {
    int len = strlen(input);
    if (len > max_len) {
        char* truncated = (char*)malloc(max_len + 4); // Additional space for "..."
        if (!truncated) return NULL;
        strncpy(truncated, input, max_len);
        strcpy(truncated + max_len, "...");
        return truncated;
    }
    return strdup(input);
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
    FILE * fp =fopen(name, "w");
    int effective_size = (int)floor(size/BLOCK_SIZE);
    printf("Tamano efectivo de escritura/lectura: %d bloques\n", effective_size);
    fseek(fp, effective_size, SEEK_SET); 
    fputc('\0', fp);
    fclose(fp);
    //inicializar el archivo
    initialize_storage(storage, effective_size); 
    
    return true;
}


bool initialize_file_blocks(int effective_size, storage *file_system, block_allocated_list *file_blocks){
    int c = 0;
    block *blocks_assigned = probe_free_blocks(file_system, (int)effective_size, &c);
    if(blocks_assigned ==NULL) {
        printf("No se lograron asignar los bloques solicitads\n");
        return NULL;
    }
    file_blocks->head = NULL;
    file_blocks->tail = NULL;
    file_blocks->size = 0;


    if (c != effective_size) {
        return NULL;  
    }

    for (int i = 0; i < effective_size; i++) {
        add_node(file_blocks, blocks_assigned[i].block_id);  // Add nodes to the file_blocks list
    }

    if(file_blocks->size!=effective_size) {
        printf("No se lograron asignar los bloques solicitads\n");
        return false;
    }

    for (int i = 0; i < effective_size; i++) {
        file_system->usage_registry[blocks_assigned[i].block_id]=true;  // Add nodes to the file_blocks list
    }

    return true;
}

bool initialize_inode(int effective_size, storage *file_system, inode *new_inode) {
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
    new_inode->size = effective_size * BLOCK_SIZE;
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
    int blocks_to_asign = (int)ceil((float)size/BLOCK_SIZE);
    printf("Effective size is: %d\n", blocks_to_asign);

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

    if(!initialize_inode(blocks_to_asign, file_system, file_inode)){
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



void clean_up_file(file *file){

}















#endif // !DEBUG
