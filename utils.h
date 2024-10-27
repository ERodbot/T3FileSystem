#ifndef UTILS
#define UTILS
// Function to check if a number is prime


int is_prime(int num);
int next_prime(int x);
char* truncate_string(const char* input, int max_len);
ssize_t append_to_buffer(int fd, char *buffer, size_t buffer_size, size_t *current_pos, size_t read_size, off_t file_offset);

int is_prime(int num) {
    if (num <= 1) {
        return 0; // Not prime
    }
    if (num == 2) {
        return 1; // 2 is the only even prime number
    }
    if (num % 2 == 0) {
        return 0; // Other even numbers are not prime
    }

    // Check divisibility from 3 up to the square root of the number
    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) {
            return 0; // Not prime if divisible
        }
    }

    return 1; // Number is prime
}

// Function to find the nearest prime number greater than x
int next_prime(int x) {
    int candidate = x + 1; // Start checking from the next number
    while (!is_prime(candidate)) {
        candidate++;
    }
    return candidate;
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

ssize_t append_to_buffer(int fd, char *buffer, size_t buffer_size, size_t *current_pos, size_t read_size, off_t file_offset) {
    //Ensure there's enough space in the buffer
    //printf("\n buffer_size: %d \t, current_pos: %d \t, read_size: %d \t file_offset: %d\n", buffer_size, *current_pos, read_size, file_offset );
    if (*current_pos + read_size > buffer_size) {
        printf("Sin espacio suficiente\n");
        return -1; // Not enough space in buffer to read more data
    }

    // Use buffer + current_pos to append data to the current position in the buffer
    ssize_t bytes_read = pread(fd, buffer + *current_pos, read_size, file_offset);
    if (bytes_read >= 0) {
        *current_pos += bytes_read; // Update the current position in the buffer
        //printf("Total bytes read  = %d\n", *current_pos);
    } else if (bytes_read < 0) {
        // Handle error if pread() fails
        perror("Error leyendo el archivo\n");
        return -1;
    }

    return bytes_read;
}

ssize_t write_from_buffer(int fd, char *buffer, size_t buffer_size, size_t *current_pos, size_t write_size, off_t file_offset) {
    // Ensure there's enough space in the buffer
    //printf("\n buffer_size: %d \t, current_pos: %d \t, write_size: %d \t file_offset: %d\n", buffer_size, *current_pos, write_size, file_offset );
    if (*current_pos + write_size > buffer_size) {
        printf("Sin espacio suficiente\n");
        return -1; // Not enough space in buffer to read more data
    }

    // Use buffer + current_pos to append data to the current position in the buffer
    ssize_t bytes_written = pwrite(fd, buffer + *current_pos, write_size, file_offset);
    if (bytes_written >= 0) {
        *current_pos += bytes_written; // Update the current position in the buffer
        //printf("Total bytes read  = %d\n", *current_pos);
    } else if (bytes_written < 0) {
        // Handle error if pread() fails
        perror("Error escribiendo el archivo\n");
        return -1;
    }

    return bytes_written;
}


bool create_and_prefill_file(const char *filename, size_t max_size, char fill_char) {
    // Open or create the file
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error creando el archivo");
        return false;
    }

    // Allocate a buffer to write the initial content
    char *buffer = (char *)malloc(max_size);
    if (buffer == NULL) {
        perror("Error asignando memoria");
        close(fd);
        return false;
    }

    // Fill the buffer with the specified character
    memset(buffer, fill_char, max_size);

    // Write the buffer to the file to pre-fill it
    ssize_t bytes_written = write(fd, buffer, max_size);
    if (bytes_written < 0 || (size_t)bytes_written != max_size) {
        perror("Error escribiendo en el archivo");
        free(buffer);
        close(fd);
        return false;
    }

    // Clean up
    free(buffer);
    close(fd);


    return true;
}


#endif // !UTILS
