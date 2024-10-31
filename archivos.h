#define MAX_LINES 70    // Max lines to read
#define MAX_LENGTH 256   // Max len of each line
// Función para leer líneas desde un archivo
// Descripción: Lee un archivo línea por línea y almacena cada línea en un arreglo dinámico de cadenas.
// Entrada: 
// - const char *filename - nombre del archivo a leer.
// - int *line_count - puntero a la variable donde se almacenará el número de líneas leídas.
// Salida: 
// - char ** - puntero a un arreglo de cadenas con las líneas leídas. Retorna NULL en caso de error.
char **read_lines(const char *filename, int *line_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error abriendo el archivo");
        return NULL;
    }

    char **lines = malloc(MAX_LINES * sizeof(char *));
    if (lines == NULL) {
        perror("Error al asignar memoria");
        fclose(file);
        return NULL;
    }

    char buffer[MAX_LENGTH];
    *line_count = 0;

    while (fgets(buffer, MAX_LENGTH, file)) {
        if (*line_count >= MAX_LINES) {
            printf("Maximo de lineas alcanzado\n");
            break;
        }

        // Reservar memoria para la línea y almacenarla
        lines[*line_count] = malloc(strlen(buffer) + 1);
        if (lines[*line_count] == NULL) {
            perror("Error al asignar memoria");
           // Liberar la memoria asignada antes de retornar
            for (int i = 0; i < *line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }

        strcpy(lines[*line_count], buffer);
        (*line_count)++;
    }

    fclose(file);
    return lines;
}

// Función para liberar la memoria asignada para las líneas
// Descripción: Libera la memoria utilizada por el arreglo de líneas leídas.
// Entrada: 
// - char **lines - arreglo de cadenas a liberar.
// - int line_count - número de líneas en el arreglo.
// Salida: Ninguna.
void free_lines(char **lines, int line_count) {
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
}
// Función para imprimir las líneas del archivo
// Descripción: Imprime el contenido del archivo línea por línea.
// Entrada: 
// - char **lines - arreglo de cadenas que contiene las líneas a imprimir.
// - int line_count - número de líneas en el arreglo.
// Salida: Ninguna.
void print_file(char **lines, int line_count){
  printf("\n File read contains:\n");
  for (int i = 0; i < line_count; i++) {
    printf("%s", lines[i]);
  }
}
// Función para recortar los espacios en blanco de una cadena
// Descripción: Recorta los espacios en blanco iniciales de una cadena.
// Entrada: 
// - const char *str - cadena de caracteres a recortar.
// Salida: 
// - const char* - puntero a la cadena recortada.
const char *trim_whitespace(const char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}
// Función para obtener el inicio de una cadena entre comillas
// Descripción: Encuentra la primera aparición de comillas dobles y devuelve un puntero al siguiente carácter.
// Entrada: 
// - const char *input - cadena de entrada.
// Salida: 
// - const char* - puntero al carácter después de la primera comilla doble, o NULL si no se encuentra.
const char *get_start(const char *input) {
    const char *start = strchr(input, '\"');
    return start ? start + 1 : NULL;
}
// Función para obtener el contenido entre dos comillas
// Descripción: Extrae el contenido entre dos comillas dobles.
// Entrada: 
// - const char *start - puntero al inicio del contenido.
// - const char *end - puntero al final del contenido.
// Salida: 
// - char* - contenido extraído como una nueva cadena, o NULL en caso de error.
char *get_content(const char *start, const char *end) {
    if (!start || !end || start >= end) return NULL;

    size_t length = end - start;
    char *data = (char *)malloc(length + 1);// Reservar memoria para los datos
    if (!data) return NULL;

    strncpy(data, start, length); // Copiar el contenido entre las comillas
    data[length] = '\0'; // Terminar la cadena con null

    return data;
}
// Función para obtener contenido largo entre comillas a lo largo de varias líneas
// Descripción: Extrae contenido entre comillas dobles que se extiende a lo largo de varias líneas.
// Entrada: 
// - const char *start - puntero al inicio del contenido.
// - const char *end - puntero al final del contenido.
// - char**lines - arreglo de líneas.
// - int i - índice de inicio de línea.
// - int end_i - índice de línea de finalización.
// Salida: 
// - char* - contenido extraído como una nueva cadena, o NULL en caso de error.
char *get_content_long(const char *start, const char *end, char**lines, int i, int end_i) {
    if (!start || !end || start >= end) return NULL;
    size_t length = end - start;
    char *data = (char *)malloc(length + 1);// Reservar memoria para los datos
    if (!data) return NULL;
    
    strncpy(data, start, length);// Copiar primera línea y llenar con NULL
    i++;
    
    for(i; i<=end_i; i++){// Concatenar todas las líneas siguientes a la primera línea
    	strcat(data, lines[i]);
    }

    data[length] = '\0'; // Terminar la cadena con null

    return data;
}
// Función para obtener datos desde las líneas leídas
// Descripción: Extrae datos entre comillas de las líneas leídas.
// Entrada: 
// - char **data - puntero a la cadena donde se almacenarán los datos extraídos.
// - char **lines - líneas leídas.
// - int line_count - número de líneas leídas.
// - int *i - puntero al índice de la línea actual.
// Salida: Ninguna.
void get_data(char **data, char **lines, int line_count, int *i){
   const char *start = get_start(lines[*i]);
   const char *end = NULL;

   if(start){
   	end = get_start(start);
   // Si hay solo una línea de datos
   	if(end!=NULL){
   	    *data = get_content(start,end);
   	}
   	// Si hay más de una línea de datos
   	else{
   	   int end_i = *i;
   	   while(end==NULL){
   	   	end = get_start(lines[++end_i]);
   	   }
       if (end) {
           *data = get_content_long(start, end, lines, *i, end_i);
           *i = end_i;
       } else {
           printf("Error no se encontro el final de la cadena, \n");
           *data = NULL;
       }
   	} 
   } else {
        printf("Error: no se encontró el inicio de la cadena.\n");
   }
}
// Función para procesar líneas y realizar operaciones
// Descripción: Procesa cada línea del archivo y ejecuta la operación correspondiente (CREATE, WRITE, READ, DELETE, LIST).
// Entrada: 
// - char **lines - líneas leídas desde el archivo.
// - int line_count - número de líneas leídas.
// - storage *drive - sistema de almacenamiento.
// - hash_table_files files_table - tabla hash de archivos.
// Salida: Ninguna.
void process_lines(char **lines, int line_count, storage *drive, hash_table_files files_table){
  for (int i = 0; i < line_count; i++) {
    const char *trimmed_line = trim_whitespace(lines[i]);
    if (*trimmed_line != '#' && *trimmed_line != '\0' && *trimmed_line != '\n'){
      printf("\n");
      char *line_copy = strdup(trimmed_line);
      if (!line_copy) {
          printf("Error: no se pudo duplicar la línea.\n");
          continue;
      }
      char *token = strtok(line_copy, " \n");
      if(strcmp(token, "CREATE")==0){
        token = strtok(NULL, " \n");
        if (!token) {
            printf("Error: Falta el nombre del archivo en CREATE.\n");
            free(line_copy);
            continue;
        }
        char *filename = token;
        token = strtok(NULL, " \n");
        if (!token || atoi(token) <= 0) {
            printf("Error: Falta un tamaño válido en CREATE.\n");
            free(line_copy);
            continue;
        }
        int size = atoi(token);
        create_file(size, filename, drive, &files_table);         
      }
      else if (strcmp(token, "WRITE")==0){
        token = strtok(NULL, " \n");
        if (!token) {
            printf("Error: Falta el nombre del archivo en WRITE.\n");
            free(line_copy);
            continue;
        }
        char *filename = token;
        token = strtok(NULL, " \n");
        if (!token || atoi(token) < 0) {
            printf("Error: Falta un offset válido en WRITE.\n");
            free(line_copy);
            continue;
        }
        int offset = atoi(token);
        token = strtok(NULL, " \n");
	
	    char *data = NULL; 
	    get_data(&data, lines, line_count, &i);	
	 
        if (data) {
	        write_file(filename, offset, data, drive, &files_table);
	        free(data); 
	    }    else {
	        printf("Error: no se pudo extraer datos de la linea.\n");
	    }
      }
      else if (strcmp(token, "READ")==0){
        token = strtok(NULL, " \n");
        if (!token) {
            printf("Error: Falta el nombre del archivo en READ.\n");
            free(line_copy);
            continue;
        }
        char *filename = token;
        token = strtok(NULL, " \n");
        if (!token || atoi(token) < 0) {
            printf("Error: Falta un offset válido en READ.\n");
            free(line_copy);
            continue;
        }
        int offset = atoi(token);
        token = strtok(NULL, " \n");
        if (!token || atoi(token) <= 0) {
            printf("Error: Falta un tamaño válido en READ.\n");
            free(line_copy);
            continue;
        }
        int size = atoi(token);
        read_file(filename, offset, size, drive, &files_table);
      }
      else if (strcmp(token, "DELETE")==0){
        token = strtok(NULL, " \n");
        if (!token) {
            printf("Error: Falta el nombre del archivo en DELETE.\n");
            free(line_copy);
            continue;
        }
        delete_file(token, drive, &files_table);
      }
      else if (strcmp(token, "LIST")==0){
        print_files(&files_table);
      }
      
    }
  }
}


