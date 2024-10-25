#include "main.h"
#define STORAGE_SIZE 7200
#define MAX_FILES 14


int main()
{

    hashable_item_file *files_table_frames = (hashable_item_file*)malloc(sizeof(hashable_item_file)*next_prime(MAX_FILES));
    hash_table_files files_table = {next_prime(MAX_FILES), 0, files_table_frames};
    storage *drive = (storage*)malloc(sizeof(storage));
    char *filename = "/root/test.txt";
    char *filename2 = "/root/test2.txt";
    char *error= "error";

    //initialize_storage(file_system, 7, blocks);
    initialize_file_system(drive, "file-system-test", 7200);
    drive->usage_registry[3] = true;



    //test para crear archivo 1
    printf("\n Archivo 1 \n");
    
    printf("\n Archivo 1 \n");
    create_file(1200, filename, drive, &files_table); 
    printf("Aqui deberia imprimir los bloques del index 0: %s\n", print_block_list(files_table.table[0].file_ref->inode->file_blocks));
    printf("Aqui deberia imprimir el tamano en index 0 bien: %d END\n", files_table.table[0].file_ref->inode->size);
    
    printf("\n");
    printf("\n Archivo Error\n");
    create_file(8000, error, drive, &files_table); 
    
    //test para crear archivo 2
    printf("\n Archivo 2 \n");
    create_file(3000, filename2, drive, &files_table); 
    printf("Aqui deberia imprimir los bloques del index 7: %s\n", print_block_list(files_table.table[7].file_ref->inode->file_blocks));
    printf("Aqui deberia imprimir el tamano en index 7 bien: %d END\n", files_table.table[7].file_ref->inode->size);


    printf("\n");
    //test de encontrar archivos 

    printf("\nENCONTRAR ARCHIVOS\n");
    file *fp = find_file(filename, &files_table);
    file *fp2 = find_file(filename2, &files_table);

    printf("\n Archivo 1 \n");
    printf("El nombre del archivo encontrado es: %s\n", fp->filepath);
    printf("El tamano del archivo encontrado es: %d\n", fp->inode->size);
    printf("La lista de bloques del archivo encontrado es: %s\n", print_block_list(fp->inode->file_blocks));

    printf("\n");

    printf("\n Archivo 2 \n");
    printf("El nombre del archivo encontrado es: %s\n", fp2->filepath);
    printf("El tamano del archivo encontrado es: %d\n", fp2->inode->size);
    printf("La lista de bloques del archivo encontrado es: %s\n", print_block_list(fp2->inode->file_blocks));
    
    
    print_files(&files_table);


    clean_up_storage(drive);


    
    /*
       int i[] = {0,1,2,3,4,5,8,9,10,11};
       char *c;
       c = (char*)malloc(sizeof(int)*10 * (10+2));


       FILE *fd = fopen("test.txt", "r");
    fseek(fd, 1, SEEK_SET);
    printf("character is %c\n", fgetc(fd));
    fclose(fd);

    int_array_to_string(i, 10,c);
    printf("resultado es: [%s]\n", c);
     *
    */

    return 0;
}
