#ifndef UTILS
#define UTILS

// Función para verificar si un número es primo
// Descripción: Comprueba si un número dado es primo.
// Entrada:
// - int num: número a verificar.
// Salida:
// - int: 1 si el número es primo, 0 si no lo es.
int is_prime(int num) {
    if (num <= 1) {
        return 0; // No es primo
    }
    if (num == 2) {
        return 1; // 2 es el único número par que es primo
    }
    if (num % 2 == 0) {
        return 0; // Otros números pares no son primos
    }

    // Verifica divisibilidad desde 3 hasta la raíz cuadrada del número
    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) {
            return 0; // No es primo si es divisible
        }
    }
    return 1; // Es primo
}

// Función para encontrar el número primo más cercano mayor que x
// Descripción: Encuentra el número primo más cercano que sea mayor a x.
// Entrada:
// - int x: número base para iniciar la búsqueda del siguiente primo.
// Salida:
// - int: número primo más cercano mayor que x.
int next_prime(int x) {
    int candidate = x + 1; // Comienza a buscar desde el siguiente número
    while (!is_prime(candidate)) {
        candidate++;
    }
    return candidate;
}

// Función auxiliar para truncar cadenas de texto largas
// Descripción: Trunca una cadena si su longitud excede el máximo especificado y agrega "..." al final.
// Entrada:
// - const char* input: cadena a truncar.
// - int max_len: longitud máxima permitida para la cadena.
// Salida:
// - char*: cadena truncada con "..." al final, o una copia de la original si es más corta que max_len.
char* truncate_string(const char* input, int max_len) {
    int len = strlen(input);
    if (len > max_len) {
        char* truncated = (char*)malloc(max_len + 4); // Espacio adicional para "..."
        if (!truncated) return NULL;
        strncpy(truncated, input, max_len);
        strcpy(truncated + max_len, "...");
        return truncated;
    }
    return strdup(input);
}

// Función para añadir contenido a un búfer desde un archivo
// Descripción: Lee datos de un archivo y los añade al búfer en la posición actual.
// Entrada:
// - int fd: descriptor del archivo.
// - char* buffer: búfer donde se almacenarán los datos leídos.
// - size_t buffer_size: tamaño del búfer.
// - size_t* current_pos: puntero a la posición actual en el búfer.
// - size_t read_size: cantidad de datos a leer.
// - off_t file_offset: offset de inicio de lectura en el archivo.
// Salida:
// - ssize_t: cantidad de bytes leídos, o -1 en caso de error.
ssize_t append_to_buffer(int fd, char *buffer, size_t buffer_size, size_t *current_pos, size_t read_size, off_t file_offset) {
    if (*current_pos + read_size > buffer_size) {
        printf("Sin espacio suficiente\n");
        return -1; // No hay suficiente espacio en el búfer
    }

    ssize_t bytes_read = pread(fd, buffer + *current_pos, read_size, file_offset);
    if (bytes_read >= 0) {
        *current_pos += bytes_read; // Actualiza la posición actual en el búfer
    } else if (bytes_read < 0) {
        perror("Error leyendo el archivo\n");
        return -1;
    }

    return bytes_read;
}

// Función para escribir desde un búfer a un archivo
// Descripción: Escribe datos desde un búfer a un archivo en la posición actual.
// Entrada:
// - int fd: descriptor del archivo.
// - char* buffer: búfer desde el cual se escribirán los datos.
// - size_t buffer_size: tamaño del búfer.
// - size_t* current_pos: puntero a la posición actual en el búfer.
// - size_t write_size: cantidad de datos a escribir.
// - off_t file_offset: offset de inicio de escritura en el archivo.
// Salida:
// - ssize_t: cantidad de bytes escritos, o -1 en caso de error.
ssize_t write_from_buffer(int fd, char *buffer, size_t buffer_size, size_t *current_pos, size_t write_size, off_t file_offset) {
    if (*current_pos + write_size > buffer_size) {
        printf("Sin espacio suficiente\n");
        return -1; // No hay suficiente espacio en el búfer
    }

    ssize_t bytes_written = pwrite(fd, buffer + *current_pos, write_size, file_offset);
    if (bytes_written >= 0) {
        *current_pos += bytes_written; // Actualiza la posición actual en el búfer
    } else if (bytes_written < 0) {
        perror("Error escribiendo el archivo\n");
        return -1;
    }

    return bytes_written;
}

// Función para crear y prellenar un archivo
// Descripción: Crea un archivo con el nombre y tamaño especificado, y lo llena con un carácter dado.
// Entrada:
// - const char* filename: nombre del archivo a crear.
// - size_t max_size: tamaño máximo del archivo.
// - char fill_char: carácter con el cual se llenará el archivo.
// Salida:
// - bool: true si el archivo se creó y prellenó con éxito, false en caso de error.
bool create_and_prefill_file(const char *filename, size_t max_size, char fill_char) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error creando el archivo");
        return false;
    }

    char *buffer = (char *)malloc(max_size);
    if (buffer == NULL) {
        perror("Error asignando memoria");
        close(fd);
        return false;
    }

    memset(buffer, fill_char, max_size);
    ssize_t bytes_written = write(fd, buffer, max_size);
    if (bytes_written < 0 || (size_t)bytes_written != max_size) {
        perror("Error escribiendo en el archivo");
        free(buffer);
        close(fd);
        return false;
    }

    free(buffer);
    close(fd);
    return true;
}

// Función para crear un arreglo lleno de un carácter específico
// Descripción: Crea un arreglo del tamaño especificado y lo llena con un carácter dado.
// Entrada:
// - int size: tamaño del arreglo a crear.
// - char x: carácter con el cual llenar el arreglo.
// Salida:
// - char*: puntero al arreglo creado, o NULL en caso de error.
char *create_x_char_filled_array(int size, char x) {
    char *x_array = (char*)malloc(sizeof(char) * size);
    if (x_array == NULL) {
        return NULL;
    }

    for (int i = 0; i < size - 1; i++) {
        x_array[i] = x;
    }
    x_array[size - 1] = '\0';
    return x_array;
}

// Función para obtener la fecha actual
// Descripción: Obtiene la fecha actual en formato "dd-mm-aaaa".
// Entrada: Ninguna.
// Salida:
// - char*: cadena con la fecha actual, o NULL en caso de error.
char* get_current_date() {
    char *date_str = (char*)malloc(11 * sizeof(char));
    if (date_str == NULL) {
        perror("Error asignando memoria");
        return NULL;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    strftime(date_str, 11, "%d-%m-%Y", &tm);
    return date_str;
}

#endif // !UTILS

