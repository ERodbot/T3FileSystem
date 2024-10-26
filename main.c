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


    //Inicio del file system
    initialize_file_system(drive, "file-system-test.txt", 7200);
    drive->usage_registry[3] = true;


    //Tes de creacion de archivos

    //test para crear archivo 1
    printf("\n Archivo 1 \n");
    
    printf("\n Archivo 1 \n");
    create_file(1200, filename, drive, &files_table); 
    printf("Aqui deberia imprimir los bloques del index 0: %s\n", print_block_list(files_table.table[0].file_ref->inode->file_blocks));
    printf("Aqui deberia imprimir el tamano en index 0 bien: %d END\n", files_table.table[0].file_ref->inode->size);
    
    printf("\n");
    printf("\n Archivo Error\n");
    create_file(8000, error, drive, &files_table); 

    printf("\n");
    print_files(&files_table);
    printf("\n");
    
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

    
    printf("\n");
    print_files(&files_table);
    printf("\n");


    //Test de lectura de archivos  
    //

    read_file(filename, 0, 40, drive, &files_table);

    printf("\n");
    print_files(&files_table);
    printf("\n");

    write_file(filename, 1500, "CARTA 9 A el señor Savilles, Inglaterra", drive, &files_table);

    read_file(filename, 0, 40, drive, &files_table);

    clean_up_storage(drive);


    

    return 0;
}
