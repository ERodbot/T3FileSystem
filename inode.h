#ifndef INODE_H 
#define INODE_H

#define BLOCK_SIZE 512
#define USER "usuario123"

struct block_allocated_list;

typedef struct {
    int size;       // Tamaño del archivo en bytes
    char *created_by;
    char *last_modified;
    char *created_at;
    char *last_access;
    block_allocated_list * file_blocks;
} inode;  

typedef struct file{
    char *filepath;
    inode *inode;
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
bool clean_up_file(file *file, storage *storage, hash_table_files *hash_t);
void delete_file(char *filepath, storage *file_system, hash_table_files *hash_t);
file *find_file(char *filepath, hash_table_files *hash_t);
unsigned long hash1(const char *key, int hash_table_size);
unsigned long hash2(const char *key, int hash_table_size);
bool initialize_file(file *new_file, char *filepath, inode *inode, hash_table_files *hash_t);
bool initialize_file_blocks(int effective_size, storage *file_system, block_allocated_list *file_blocks);
bool initialize_file_system(storage *storage, char *name, int size);
bool initialize_inode(int effective_size, storage *file_system, inode *new_inode);
bool insert_into_hashtable(file *file, hash_table_files *hash_t);
char* prepend_filesystem_name(char* filepath, storage* file_system);
void print_files(hash_table_files *hash_t);
void read_file(char *filepath, size_t offset, size_t amount, storage *file_system, hash_table_files *hash_t);
bool remove_from_hashtable(file *file, hash_table_files *hash_t);
bool write_file(char *filepath, size_t offset, char *data, storage *file_system, hash_table_files *hash_t);

// Función de hash djb2
// Descripción: Genera un valor hash a partir de una clave dada.
// Entrada: const char *key - clave de la cual se calculará el hash; int hash_table_size - tamaño de la tabla hash
// Salida: unsigned long - valor hash calculado.
unsigned long  hash1 (const char *key, int hash_table_size){
    unsigned long hash = 5381;
    int c; 
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) +c;
    }
    return hash % hash_table_size; 
}
// Función de hash FNV-1a
// Descripción: Genera un valor hash a partir de una clave dada.
// Entrada: const char *key - clave de la cual se calculará el hash; int hash_table_size - tamaño de la tabla hash
// Salida: unsigned long - valor hash calculado.
unsigned long hash2 (const char *key, int hash_table_size) {
    unsigned long hash = 2166136261U; 
    int c; 
    while ((c = *key++)){
        hash = (hash^c)* 16777619;
    }
    return (hash % (hash_table_size-1)) +1;
}
// Función para buscar un archivo en la tabla hash usando doble hash
// Descripción: Busca un archivo por su ruta utilizando un esquema de doble hash.
// Entrada: char *filepath - ruta del archivo a buscar; hash_table_files *hash_t - tabla hash de archivos
// Salida: file* - puntero al archivo encontrado, o NULL si no se encuentra.
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
// Función para insertar un archivo en la tabla hash con doble hash
// Descripción: Inserta un archivo en la tabla hash utilizando doble hash para la gestión de colisiones.
// Entrada: file *file - archivo a insertar; hash_table_files *hash_t - tabla hash de archivos
// Salida: bool - True si la inserción fue exitosa, False en caso contrario.
bool insert_into_hashtable(file *file, hash_table_files *hash_t){
    unsigned long hash_1 = hash1(file->filepath, hash_t->size);
    unsigned long hash_2 = hash2(file->filepath, hash_t->size);
    hashable_item_file *hash_table = hash_t->table;
    int index = (int)hash_1;

    int c = 0;

    while(hash_table[index].flag==1) { // Verificar si la entrada está ocupada
        if (index==hash_1){
            printf("Fallo el insertado en tabla de archivos\n");
            return false;
        }
        if (c > hash_t->size) {
            printf("No existe mas espacio para achivos en el sistema\n");
            return false;
        } else {
            index = (hash_1 + c* hash_2) % hash_t->size;
            c++;
        }
    }
    // Asignar referencia del archivo y marcar la entrada como ocupada
    hash_table[index].file_ref= file;
    hash_table[index].flag = 1;
    hash_t->current_occupied++;
    return true;
}
// Función para remover un archivo de la tabla hash
// Descripción: Elimina un archivo de la tabla hash desmarcando la entrada correspondiente.
// Entrada: file *file - archivo a eliminar; hash_table_files *hash_t - tabla hash de archivos
// Salida: bool - True si la eliminación fue exitosa, False en caso contrario
bool remove_from_hashtable(file *file, hash_table_files *hash_t){
    unsigned long hash_1 = hash1(file->filepath, hash_t->size);
    unsigned long hash_2 = hash2(file->filepath, hash_t->size);
    hashable_item_file *hash_table = hash_t->table;
    int index = (int)hash_1;
    int c = 0;

    while(hash_table[index].flag==1) { // Verificar si la entrada está ocupada
        if (strcmp(hash_table[index].file_ref->filepath, file->filepath)==0){ 
           printf("Archivo: %s encontrado, removiendo su referencia\n", file->filepath);
           hash_table[index].file_ref=NULL;
           hash_table[index].flag= 0;
           hash_t->current_occupied--;
           return true;
        }
        index = (hash_1 + c* hash_2) % hash_t->size;
        c++;
    }
    
    printf("Archivo: %s no encontrado,  no se pudo remover su referencia\n", file->filepath);
    return false;
}
// Función para imprimir los archivos en la tabla hash
// Descripción: Imprime todos los archivos en la tabla hash junto con sus metadatos.
// Entrada: hash_table_files *hash_t - tabla hash de archivos
// Salida: Ninguna. Imprime la información de los archivos en la consola.void print_files(hash_table_files *hash_t) {
void print_files(hash_table_files *hash_t) {
    hashable_item_file *hash_table = hash_t->table;
    int i;
    printf("\n\n╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗");
    printf("\n║                                               Archivos del Sistema                                                        ║");
    printf("\n╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("╔═════╦══════════════════════════════════════════╦═════════════════════════════════════════════════════╦═══════════════════╦══════════════╦══════════════════╦═════════════════════╦═════════════════════╗\n");
    printf("║  Id ║ Nombre                                   ║ Bloques Asignados                                   ║ Tamaño en bytes   ║ Creado por   ║ Fecha de creación║ Último acceso       ║ Última modificación ║\n");
    printf("╠═════╬══════════════════════════════════════════╬═════════════════════════════════════════════════════╬═══════════════════╬══════════════╬══════════════════╬═════════════════════╬═════════════════════╣\n");
    
    for (i = 0; i < hash_t->size; i++) {
        if (hash_table[i].flag == 1) {
            char *filepath = truncate_string(hash_table[i].file_ref->filepath, 40); // Limita la longitud de la ruta a 40 caracteres
            file *current_file = find_file(filepath, hash_t);
            block_allocated_list *printable_list = current_file->inode->file_blocks;
            char *block_l = truncate_string(print_block_list(printable_list), 50); // Limita la longitud de la lista de bloques a 50 caracteres
            
            // Print formatted row with alignment
            printf("║ %-3d ║ %-40s ║ %-51s ║ %-17d ║ %-12s ║ %-16s ║ %-19s ║ %-19s ║\n", 
                   i, 
                   filepath, 
                   block_l, 
                   current_file->inode->size,
                   current_file->inode->created_by,
                   current_file->inode->created_at,
                   current_file->inode->last_access,
                   current_file->inode->last_modified);
            printf("╠═════╬══════════════════════════════════════════╬═════════════════════════════════════════════════════╬═══════════════════╬══════════════╬══════════════════╬═════════════════════╬═════════════════════╣\n");
            
            // Free allocated memory
            free(filepath);
            free(block_l);
        }
    }
    
    printf("╚═════╩══════════════════════════════════════════╩═════════════════════════════════════════════════════╩═══════════════════╩══════════════╩══════════════════╩═════════════════════╩═════════════════════╝\n");
}
// Función para inicializar el sistema de archivos
// Descripción: Inicializa un sistema de archivos con un nombre y un tamaño específico.
// Entrada: storage *storage - puntero al sistema de almacenamiento; char *name - nombre del sistema; int size - tamaño en bloques
// Salida: bool - True si la inicialización fue exitosa, False en caso contrario.
bool initialize_file_system(storage *storage, char *name, int size){
    //inicializar archivo para simulacion de filesystem;
    if(!create_and_prefill_file(name, size, ' ')){
            return false;
    };
    int effective_size = (int)floor(size/BLOCK_SIZE);
    printf("Tamano efectivo de escritura/lectura: %d bloques\n", effective_size);
    //inicializar la estructura de almacenamiento
    if(!initialize_storage(storage, effective_size, name)){
        return false;
    }; 
    
    return true;
}
// Función para inicializar una lista de bloques para un archivo
// Descripción: Inicializa una lista de bloques para un archivo y asigna los bloques necesarios.
// Entrada: int effective_size - cantidad de bloques necesarios; storage *file_system - sistema de almacenamiento; block_allocated_list *file_blocks - lista de bloques a inicializar
// Salida: bool - True si la inicialización fue exitosa, False en caso contrario.
bool initialize_file_blocks(int effective_size, storage *file_system, block_allocated_list *file_blocks){
    file_blocks->tail = NULL;
    file_blocks->head = NULL;
    file_blocks->size = 0;

    // Extiende la lista de bloques asignados
    return extend_block_allocated_list(file_system, file_blocks, effective_size);
}
// Función para inicializar un inodo
// Descripción: Inicializa un inodo con los bloques asignados y los metadatos básicos.
// Entrada: int size - tamaño del archivo; storage *file_system - sistema de almacenamiento; inode *new_inode - inodo a inicializar
// Salida: bool - True si la inicialización fue exitosa, False en caso contrario.
bool initialize_inode(int size, storage *file_system, inode *new_inode) {
    int effective_size= (int)ceil((float)size/BLOCK_SIZE);
    int i;
    block_allocated_list *file_blocks = (block_allocated_list*)malloc(sizeof(block_allocated_list));
    if(file_blocks==NULL) {
        printf("No se lograron asignar los bloques solicitads\n");
        return false;
    }

    if(!initialize_file_blocks(effective_size, file_system, file_blocks)) {
        printf("No se lograron asignar los bloques solicitados\n");
        free(file_blocks);
        return false;
    }
    
    // Asigna los atributos del inodo
    new_inode->size = size;
    new_inode->created_by = USER;             
    new_inode->last_modified = "No aplica";   
    new_inode->created_at = get_current_date();      
    new_inode->last_access = "No aplica";     
    new_inode->file_blocks = file_blocks;    

    return true;
}
// Función para inicializar un archivo
// Descripción: Inicializa un archivo con la ruta dada y el inodo asociado, y lo inserta en la tabla hash.
// Entrada: file *new_file - archivo a inicializar; char *filepath - ruta del archivo; inode *inode - inodo del archivo; hash_table_files *hash_t - tabla hash de archivos
// Salida: bool - True si la inicialización fue exitosa, False en caso contrario.
bool initialize_file(file *new_file, char *filepath, inode *inode, hash_table_files *hash_t){
    new_file->filepath=filepath;
    new_file->inode=inode;

    if(!insert_into_hashtable(new_file, hash_t)){
        printf("Error generando archivo \n"); 
        return false;
    }

    return true;
}
// Función para añadir el nombre del sistema de archivos a una ruta de archivo
// Descripción: Precede la ruta de un archivo con el nombre del sistema de archivos.
// Entrada: char *filepath - ruta del archivo; storage *file_system - sistema de almacenamiento
// Salida: char* - nueva ruta de archivo con el nombre del sistema añadido.
char* prepend_filesystem_name(char* filepath, storage* file_system) {
    int prefix_length = strlen(file_system->name) + 1; 
    int new_length = prefix_length + strlen(filepath) + 1; 

    char *new_filepath = (char*)malloc(new_length * sizeof(char));
    if (new_filepath == NULL) {
        printf("Error asignando memoria para el nuevo nombre de archivo\n");
        return NULL;
    }

    snprintf(new_filepath, new_length, "%s:%s", file_system->name, filepath);
    
    return new_filepath;
}
// Función para crear un archivo en el sistema de archivos
// Descripción: Crea un archivo con un tamaño específico, inicializa su inodo y lo agrega a la tabla hash.
// Entrada: int size - tamaño del archivo; char *filepath - ruta del archivo; storage *file_system - sistema de almacenamiento; hash_table_files *hash_t - tabla hash de archivos
// Salida: bool - True si la creación fue exitosa, False en caso contrario.
bool create_file(int size, char* filepath, storage *file_system, hash_table_files *hash_t) {
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n");
    printf("Iniciando creacion de archivo: %s \n", filepath);
    char *s = prepend_filesystem_name(filepath, file_system);
    if (s == NULL) {
        printf("Error asignando memoria\n");
        return false;
    }
    
    inode *file_inode = (inode*)malloc(sizeof(inode));   
    if (file_inode == NULL) {
        printf("No se pudo asignar una estructura esencial  para crear el archivo\n");
        free(s);
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return false;
    }

    if(!initialize_inode(size, file_system, file_inode)){
        printf("No se pudo inicializar una esturctura escencial para crear el archivo\n");
        free(file_inode);
        free(s);
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return false;
    }

    file *new_file = (file*)malloc(sizeof(file));

    //insertar el archivo en la tabla hash 
    if(!initialize_file(new_file,s,file_inode, hash_t)){
        printf("Archivo no pudo ser inicializado\n");
        clean_up_file(new_file, file_system, hash_t);
        free(new_file->filepath);
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n");
        return false;
    }
    printf("Archivo generado con exito\n");
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
    return true;
}
// Función para extender un archivo si es necesario
// Descripción: Extiende un archivo si se necesita más espacio para escribir datos adicionales.
// Entrada: int offset - posición de inicio; size_t data_size - tamaño de los datos a escribir; file *file_written - archivo a extender; storage *file_system - sistema de almacenamiento
// Salida: bool - True si se pudo extender, False si no.
bool extend_file(int offset, size_t data_size, file *file_written, storage*file_system){
    int surplus = (offset + data_size) - file_written->inode->size;
    // Calcular la cantidad de bloques necesarios
    int required_blocks = (int)ceil((float)(file_written->inode->size + surplus) / BLOCK_SIZE);
    int current_blocks = file_written->inode->file_blocks->size;
    printf("Bloques actuales: %d y bloques necesarios: %d\n", current_blocks, required_blocks);
    if (surplus > 0 && required_blocks > current_blocks) {
        /// Extender los bloques asignados si es necesario
        if (!extend_block_allocated_list(file_system, file_written->inode->file_blocks, required_blocks-current_blocks)) {
            printf("Error, no se pudo extender el archivo para escribir sobre él\n");
            return false;
        }
        printf("Se asignaron más bloques de memoria al archivo\n");
        file_written->inode->size += surplus;
    }

    return true;
}
// Función para escribir datos en un archivo
// Descripción: Escribe datos en un archivo específico a partir de un offset dado y extiende el archivo si es necesario.
// Entrada: 
// - char *filepath - Ruta del archivo donde se escribirán los datos.
// - size_t offset - Desplazamiento desde el inicio del archivo donde se iniciará la escritura.
// - char *data - Datos que se escribirán en el archivo.
// - storage *file_system - Sistema de almacenamiento donde reside el archivo.
// - hash_table_files *hash_t - Tabla hash que contiene las referencias a los archivos.
// Salida: Ninguna. Realiza la escritura de los datos en el archivo o muestra mensajes de error si falla.
bool write_file(char *filepath, size_t offset, char *data, storage *file_system, hash_table_files *hash_t) {
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n");
    // Localizar el archivo en la tabla hash
    printf("Iniciando escritura de archivo: %s \n", filepath);
    file *file_written = find_file(filepath, hash_t);
    if (file_written == NULL) {
        printf("Error, no se pudo encontrar el archivo\n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return false;
    }

    // Validar que la posición de escritura esté dentro del límite del archivo
    if (file_written->inode->size < offset) {
        printf("Error, no se puede escribir en una posición fuera del límite del archivo\n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return false;
    }

    // Determinar el tamaño de los datos a escribir
    size_t data_size = strlen(data);
    if (!extend_file(offset, data_size, file_written, file_system)) {
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return false;
    }

    off_t starting_block_position = (int)floor((double)offset / BLOCK_SIZE);
    off_t starting_point_in_block = offset % BLOCK_SIZE;
    size_t batch_size = MIN(data_size, MIN((BLOCK_SIZE - starting_point_in_block), (file_written->inode->size - offset)));
    size_t total_bytes_written = 0;
    block_node *current_block_node = get_node_at_position(file_written->inode->file_blocks, starting_block_position);
    if (current_block_node == NULL) {
        printf("Error: el archivo no posee espacio asignado en memoria\n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return false;
    }

    block *current_block = &file_system->memory_blocks[current_block_node->block_id];
    if (current_block == NULL) {
        printf("Error: no se encontro una estructura esencial para la escritura en el archivo \n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return false;
    }

    int fd = open(file_system->name, O_WRONLY);
    if (fd < 0) {
        printf("Sistema de archivos corrupto, abortando\n");
        close(fd);
        abort();
    }

   // Escribir datos en el archivo por lotes (batches)
    while (current_block_node != NULL && (data_size - total_bytes_written) != 0) {
       // Actualizar el bloque actual
        current_block = &file_system->memory_blocks[current_block_node->block_id];

        // Escribir datos en el archivo desde el buffer
        if (write_from_buffer(fd, data, data_size, &total_bytes_written, batch_size,
                              (off_t)current_block->start_offset + starting_point_in_block) < 0) {
            close(fd);
            printf("Error escribiendo el archivo, cancelando operación\n");
            return false;
        }

        // Calcular el siguiente tamaño de lote y actualizar el offset dentro del bloque
        batch_size = MIN(data_size - total_bytes_written, BLOCK_SIZE);
        starting_point_in_block = 0;  // Después del primer lote, el offset dentro del bloque es cero

       // Moverse al siguiente bloque en la lista
        current_block_node = current_block_node->next;
    }
    // Actualizar los metadatos del archivo después de la escritura
    file_written->inode->last_modified = get_current_date();
    file_written->inode->last_access = get_current_date();

    // Close the file descriptor after writing
    close(fd);
    printf("\nArchivo escrito  con exito\n");
    return true;
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
}
// Función para leer un archivo desde el sistema de archivos
// Descripción: Lee una cantidad específica de datos desde un archivo, empezando en un desplazamiento dado.
// Entrada: char *filepath - ruta del archivo; size_t offset - desplazamiento de inicio; size_t amount - cantidad de datos a leer; storage *file_system - sistema de almacenamiento; hash_table_files *hash_t - tabla hash de archivos
// Salida: Ninguna. Imprime el contenido leído del archivo o un mensaje de error.
void read_file(char *filepath, size_t offset, size_t amount, storage *file_system, hash_table_files *hash_t){
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n");
    printf("Empezando a leer el archivo: %s\n", filepath);
    file *file_read = find_file(filepath, hash_t);
    if (file_read==NULL){
        printf("Error, no se pudo encontrar el archivo\n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return;
    }
    // Verificar si la posición de lectura es válida
    if (file_read->inode->size < offset+amount){
        printf("Error, no se puede leer en una posicion fuera del limite del archivo\n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return;
    } 
    off_t starting_block_position = (int)floor(offset/BLOCK_SIZE);
    off_t starting_point_in_block =  offset - BLOCK_SIZE*starting_block_position;
    size_t batch_size = MIN(amount, MIN((BLOCK_SIZE - starting_point_in_block), (file_read->inode->size - offset)));
    size_t total_bytes_read = 0;
    block_node *block_node_read_currently = get_node_at_position(file_read->inode->file_blocks, starting_block_position);
    block *block_read_currently = &file_system->memory_blocks[block_node_read_currently->block_id];

    if (block_read_currently == NULL) {
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return; 
    }

    char *buffer_content_read = (char*)malloc(sizeof(char)*amount + 1);
    if (buffer_content_read==NULL){
        printf("Error: no se pudo asignar memoria\n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
        return;
    }

    int fd = open(file_system->name, O_RDONLY);
    if(fd < 0){
        printf("Sistema de archivos corrupto, abortando \n");
        close(fd);
        abort();
    }

    while(block_node_read_currently!=NULL && (amount-total_bytes_read)!=0){
        // Leer contenido del archivo desde el buffer
        block_read_currently = &file_system->memory_blocks[block_node_read_currently->block_id];
        if(append_to_buffer(fd, buffer_content_read, (size_t) amount + 1, &total_bytes_read, batch_size, (off_t)block_read_currently->start_offset + starting_point_in_block) < 0){
            printf("Error leyendo el archivo, cancelando operacion");
            free(buffer_content_read);
            close(fd);
            return;
        } 
        batch_size = (amount - total_bytes_read >= BLOCK_SIZE) ? BLOCK_SIZE : (amount - total_bytes_read);
        starting_point_in_block = 0;
        block_node_read_currently = block_node_read_currently->next;
    }

    buffer_content_read[amount] = '\0';
    printf("\n Contenido: \t %s \n", buffer_content_read);
    free(buffer_content_read); 
    file_read->inode->last_access = get_current_date();
    printf("\n\n Fin del archivo: \n");
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
}
// Función para eliminar un archivo del sistema de archivos
// Descripción: Elimina un archivo del sistema de archivos, limpia su contenido y libera la memoria asignada.
// Entrada: char *filepath - ruta del archivo; storage *file_system - sistema de almacenamiento; hash_table_files *hash_t - tabla hash de archivos
// Salida: Ninguna.
void  delete_file(char *filepath, storage *file_system, hash_table_files *hash_t){
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n");
    printf("Iniciando borrado de archivo: %s \n", filepath);
    file *file_deleted = find_file(filepath, hash_t);
    if(!file_deleted){
        printf("\nArchivo no encontrado\n");
        printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
       return;
    }
    // Limpia el archivo y lo elimina de la tabla hash
    if(!clean_up_file(file_deleted, file_system, hash_t)){
        return;
    }
    if(!remove_from_hashtable(file_deleted, hash_t)){
        return;
    }
    // Libera la memoria de la estructura del archivo
    free(file_deleted->filepath);
    free(file_deleted);
    printf("\nArchivo borrado con exito\n");
    printf("══════════════════════════════════════════════════════════════════════════════════════════════════════════\n\n");
}
// Función para limpiar un archivo en el sistema de archivos
// Descripción: Elimina los bloques asignados y los metadatos de un archivo, y lo borra de la tabla hash.
// Entrada: file *file - archivo a limpiar; storage *file_system - sistema de almacenamiento; hash_table_files *hash_t - tabla hash de archivos
// Salida: Ninguna.
bool clean_up_file(file *file, storage *file_system, hash_table_files *hash_t){
    inode *file_inode = file->inode;
    block_allocated_list *file_blocks  = file_inode->file_blocks;
    block_node *temp = file_blocks->tail;
    printf("tamano de archivo es: %d\n", file_inode->size);
    char *overwrite = create_x_char_filled_array(file_inode->size, ' ');
    if (overwrite == NULL) {
        printf("Error asignando memoria\n");
        return false;
    }
    printf("tamano de escritura para borrado es: %d\n", strlen(overwrite));
    write_file(file->filepath, 0, overwrite, file_system, hash_t);
        
    while(file_blocks->size !=0 & temp!=NULL){
        file_system->usage_registry[temp->block_id] = false;
        temp = temp->prev;
        remove_node_at_end(file_blocks);
    }
    //limpiar los archivos
    free(overwrite);
    free(file_inode->file_blocks);
    free(file_inode->last_access);
    free(file_inode->created_at);
    free(file_inode->last_modified);
    free(file_inode);
    return true;
}


// Función para limpiar la tabla hash de archivos
// Descripción: Libera la memoria de la tabla hash y de cada archivo almacenado en ella.
// Entrada:
// - storage *file_system - Puntero al sistema de almacenamiento donde se encuentran los archivos.
// - hash_table_files *hash_t - Puntero a la tabla hash de archivos que será limpiada.
// Salida: Ninguna.
void clean_up_hash_table(storage *file_system, hash_table_files *hash_t) {
    if (hash_t == NULL) {
        printf("No se puede limpiar una tabla de archivos inexistente \n");
        return;
    }

    // Recorrer cada entrada de la tabla hash
    for (int i = 0; i < hash_t->size; i++) {
        if (hash_t->table[i].flag == 1) { // Verificar si la entrada está ocupada
            // Llamar a delete_file con la ruta del archivo referenciado
            delete_file(hash_t->table[i].file_ref->filepath, file_system, hash_t);
        }
    }

    // Liberar la memoria del array de la tabla hash y de la estructura hash_table_files
    free(hash_t->table);
    //free(hash_t);
}











#endif // !DEBUG

