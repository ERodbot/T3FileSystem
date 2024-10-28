#define MAX_LINES 50    // Max lines to read
#define MAX_LENGTH 256   // Max len of each line

char **read_lines(const char *filename, int *line_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    char **lines = malloc(MAX_LINES * sizeof(char *));
    if (lines == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    char buffer[MAX_LENGTH];
    *line_count = 0;

    while (fgets(buffer, MAX_LENGTH, file)) {
        if (*line_count >= MAX_LINES) {
            printf("Reached maximum line limit\n");
            break;
        }

        // Allocate memory for the line and store it
        lines[*line_count] = malloc(strlen(buffer) + 1);
        if (lines[*line_count] == NULL) {
            perror("Memory allocation failed");
            // Clean up allocated lines before returning
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

// Function to free allocated memory for lines
void free_lines(char **lines, int line_count) {
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
}

void print_file(char **lines, int line_count){
  printf("\n File read contains:\n");
  for (int i = 0; i < line_count; i++) {
    printf("%s", lines[i]);
  }
}

const char *trim_whitespace(const char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

const char *get_start(const char *input) {
    const char *start = strchr(input, '\"');
    return start ? start + 1 : NULL;
}

char *get_content(const char *start, const char *end) {
    if (!start || !end || start >= end) return NULL;

    size_t length = end - start;
    char *data = (char *)malloc(length + 1); // Allocate memory for the data
    if (!data) return NULL;

    strncpy(data, start, length); // Copy the data between quotes
    data[length] = '\0'; // Null-terminate the string

    return data;
}

char *get_content_long(const char *start, const char *end, char**lines, int i, int end_i) {
    if (!start || !end || start >= end) return NULL;
    size_t length = end - start;
    char *data = (char *)malloc(length + 1); // Allocate memory for the data
    if (!data) return NULL;
    
    strncpy(data, start, length); // Copy first line, fill with NULL
    i++;
    
    for(i; i<=end_i; i++){ //Concat all other lines to first line
    	strcat(data, lines[i]);
    }

    data[length] = '\0'; // Null-terminate the string

    return data;
}

void get_data(char **data, char **lines, int line_count, int *i){
   const char *start = get_start(lines[*i]);
   const char *end = NULL;

   if(start){
   	end = get_start(start);
   	//There is only one line of data
   	if(end!=NULL){
   	    *data = get_content(start,end);
   	}
   	//There is more than one line of data
   	else{
   	   int end_i = *i;
   	   while(end==NULL){
   	   	end = get_start(lines[++end_i]);
   	   }
   	   *data = get_content_long(start, end, lines, *i, end_i);
   	   *i = end_i;
   	} 
   }
}

void process_lines(char **lines, int line_count, storage *drive, hash_table_files files_table){
  for (int i = 0; i < line_count; i++) {
    const char *trimmed_line = trim_whitespace(lines[i]);
    if (*trimmed_line != '#' && *trimmed_line != '\0' && *trimmed_line != '\n'){
    printf("\n");
      char *line_copy = strdup(trimmed_line);
      char *token = strtok(line_copy, " \n");
      if(strcmp(token, "CREATE")==0){
        token = strtok(NULL, " \n");
        char *filename = token;
        token = strtok(NULL, " \n");
        int size = atoi(token);
        create_file(size, filename, drive, &files_table);         
      }
      else if (strcmp(token, "WRITE")==0){
        token = strtok(NULL, " \n");
        char *filename = token;
        token = strtok(NULL, " \n");
        int offset = atoi(token);
        token = strtok(NULL, " \n");
	
	char *data = NULL; 
	get_data(&data, lines, line_count, &i);	
	 
        if (data) {
	   write_file(filename, offset, data, drive, &files_table);
	   free(data); 
	} else {
	   printf("Error: Could not extract data from line.\n");
	}
      }
      else if (strcmp(token, "READ")==0){
        token = strtok(NULL, " \n");
        char *filename = token;
        token = strtok(NULL, " \n");
        int offset = atoi(token);
        token = strtok(NULL, " \n");
        int size = atoi(token);
        read_file(filename, offset, size, drive, &files_table);
      }
      else if (strcmp(token, "DELETE")==0){
        token = strtok(NULL, " \n");
        delete_file(token, drive, &files_table);
      }
      else if (strcmp(token, "LIST")==0){
        print_files(&files_table);
      }
      
    }
  }
}


