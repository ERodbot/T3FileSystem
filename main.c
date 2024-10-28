#include "main.h"
#define STORAGE_SIZE 14400
#define MAX_FILES 14


int main(int argc, char **argv)
{
	
    if (argc > 3)
    {
      fprintf(stderr, "Too many arguments provided for the program's execution\n");
    }
    
    const char *filepath = argv[1];
    const int size = atoi(argv[2]);  

    hashable_item_file *files_table_frames = (hashable_item_file*)malloc(sizeof(hashable_item_file)*next_prime(MAX_FILES));
    hash_table_files files_table = {next_prime(MAX_FILES), 0, files_table_frames};
    storage *drive = (storage*)malloc(sizeof(storage));
    
    initialize_file_system(drive, "file-system-test.txt", size);
    
    
    int line_count;
    char **lines = read_lines(filepath, &line_count);
    if (lines != NULL)
    {
      process_lines(lines, line_count, drive, files_table);
      free_lines(lines, line_count);
    }
    
    clean_up_storage(drive);
    
    return 0;
}
