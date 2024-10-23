#ifndef INODE_H 
#define INODE_H


#define BINARY "bin"
#define TEXT "txt" 
#define DIRECTORY "dir" 
#define FILESYSTEM "fsystem"
#define BLOCK_SIZE 512
#define SAMPLE_DATE "10/23/2024"

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
    const int size; 
    int current_occupied;
    hashable_item_file * table;
} hash_table_files;

unsigned long hash1 (const char *key, int hash_table_size);
unsigned long hash2 (const char *key, int hash_table_size);
file *find_file(char *filepath, hash_table_files *hash_t);
bool insert_into_hashtable(file *file, hash_table_files *hash_t);
bool remove_from_hashtable(file *file, hash_table_files *hash_t);
void print_files(hash_table_files *hash_t, inode *inodes);
    


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
    int index = hash_1;
    int c = 0;

    while(hash_table[index].flag==1) {
        if (strcmp(hash_table[index].file_ref->filepath, filepath)==0){
            printf("Archivo: %s encontrado, retornado su referencia \n", filepath);
            return hash_table[index].file_ref;
        }
        index = (hash_1 + c* hash_2) % hash_t->size;
        c++;
    }

    printf("Archivo: %s no  encontrado\n", filepath);
    return NULL;
}

bool insert_into_hashtable(file *file, hash_table_files *hash_t){
    unsigned long hash_1 = hash1(file->filepath, hash_t->size);
    unsigned long hash_2 = hash2(file->filepath, hash_t->size);
    hashable_item_file *hash_table = hash_t->table;
    int index = hash_1;
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



//todo: imprime lista de bloques que le pertenecen a cada archivo;
void print_files(hash_table_files *hash_t, inode *inodes) {
    hashable_item_file *hash_table = hash_t->table;
    int i;
    char *list_nodes;
    printf("\n\n Archivos del sisrema\n");
    printf("---------------------------");
    printf("Nombre \t Bloques Asignados \t tamano del archivo");
    for(i = 0; i<hash_t->size; i++){
        if (hash_table[i].flag==1) {
            char *string_blocks = print_block_list(hash_table[i].file_ref->inode->file_blocks);
            printf("%s \t %s \t %d \t", 
                    hash_table[i].file_ref->filepath,
                    string_blocks,
                    hash_table[i].file_ref->inode->size
            );
        }
        printf("---------------------------");
    }
}


/*
bool create_file(int size, char* filepath, storage *file_system, hash_table_files *hash_t) {
    int c = 0;
    int i;
    double blocks_to_asign = ceil(size/BLOCK_SIZE);
    block *blocks_asigned = probe_free_blocks(file_system, (int)blocks_to_asign, &c);
    block_allocated_list file_blocks = {NULL, NULL, 0};
     
    
    if(c!=blocks_to_asign){
        return false;
    }
    for(i = 0; i<blocks_to_asign; i++){
        add_node(&file_blocks, blocks_asigned[i].block_id);
    }
    
    int * checksums = (int*)calloc(size, sizeof(int));

    if (checksums==NULL) {
        printf("Error: no se pudieron generar los checksums del archivo con nombre: %s ", filepath);
    }



    inode inode = {checksums,false, size, SAMPLE_DATE,SAMPLE_DATE, SAMPLE_DATE, SAMPLE_DATE, &file_blocks};

    file new_file = {filepath, &inode, false};

    //todo: meterlo a la tabla de archivos
} 

*/














#endif // !DEBUG
