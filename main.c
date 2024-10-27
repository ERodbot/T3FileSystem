#include "main.h"
#define STORAGE_SIZE 14400
#define MAX_FILES 14


int main()
{

    hashable_item_file *files_table_frames = (hashable_item_file*)malloc(sizeof(hashable_item_file)*next_prime(MAX_FILES));
    hash_table_files files_table = {next_prime(MAX_FILES), 0, files_table_frames};
    storage *drive = (storage*)malloc(sizeof(storage));
    char *filename = "/root/test.txt";
    char *filename2 = "/root/test2.txt";
    char *filename3 = "/root/test3.txt";
    char *error= "error";


    //Inicio del file system
    initialize_file_system(drive, "file-system-test.txt", 7200);


    //Tes de creacion de archivos

    //test para crear archivo 1
    printf("\n Archivo 1 \n");
    
    printf("\n Archivo 1 \n");
    create_file(1023, filename, drive, &files_table); 
    printf("Aqui deberia imprimir los bloques del index 0: %s\n", print_block_list(files_table.table[0].file_ref->inode->file_blocks));
    printf("Aqui deberia imprimir el tamano en index 0 bien: %d END\n", files_table.table[0].file_ref->inode->size);
   
    
    printf("\n");
    printf("\n Archivo Error\n");
    create_file(20000, error, drive, &files_table); 
    

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

    printf("\n");
    
    printf("\n");
    print_files(&files_table);
    printf("\n");

    //Test de lectura/escritura de archivos  

    write_file(filename, 0, "@@ ESTE ES UN CAMBIO @@", drive, &files_table);
    read_file(filename, 0, 50, drive, &files_table);

    write_file(filename, 51, "@@ ESTE ES UN CAMBIO NUMERO 2  @@", drive, &files_table);
    read_file(filename, 0, 100, drive, &files_table);

    char carta[] = "A la señora Saville, Inglaterra San Petersburgo, 11 de diciembre de 17... Te alegrarás de saber que ningún percance ha acompañado el comienzo de la empresa que tú contemplabas con tan malos presagios. Llegué aquí ayer, y mi primera obligación es tranquilizar a mi querida hermana sobre mi bienestar y comunicarle mi creciente confianza en el éxito de mi empresa. Me encuentro ya muy al norte de Londres, y andando por las calles de Petersburgo noto en las mejillas una fría brisa norteña que azuza mis nervios j me llena de alegría. ¿Entiendes este sentimiento? Esta brisa, que viene de aquellas regiones hacia las que yo me dirijo, me anticipa sus climas helados. Animado por este viento prometedor, mis esperanzas se hacen más fervientes y reales. Intento en vano convencerme de que el Polo es la morada del hielo y la desolación. Sigo imaginándomelo como la región de la hermosura y el deleite. Allí, Margaret, se ve siempre el sol, su amplio círculo rozando justo el horizonte y difundiendo un perpetuo resplandor. Allí pues con tu permiso, hermana mía, concederé un margen de confianza a anteriores navegantes, allí, no existen ni la nieve ni el hielo y navegando por un mar sereno se puede arribar a una tierra que supera, en maravillas y hermosura, cualquier región descubierta hasta el momento en el mundo habitado.";

    write_file(filename2, 0, carta, drive, &files_table);
    read_file(filename2, 0, 3000, drive, &files_table);

    delete_file(filename, drive, &files_table);

    printf("\n");
    print_files(&files_table);
    printf("\n");
    
    
    //test para crear archivo 3
    printf("\n Archivo 3 \n");
    create_file(2500, filename3, drive, &files_table); 
    printf("Aqui deberia imprimir los bloques del index 7: %s\n", print_block_list(files_table.table[7].file_ref->inode->file_blocks));
    printf("Aqui deberia imprimir el tamano en index 7 bien: %d END\n", files_table.table[7].file_ref->inode->size);

    printf("\n");

    //write_file(filename3, 0, "@@ ESTE ES UN CAMBIO @@", drive, &files_table);
    read_file(filename3, 0, 50, drive, &files_table);

    file *fp3 = find_file(filename3, &files_table);
    printf("\n Archivo 3 \n");
    printf("El nombre del archivo encontrado es: %s\n", fp3->filepath);
    printf("El tamano del archivo encontrado es: %d\n", fp3->inode->size);
    printf("La lista de bloques del archivo encontrado es: %s\n", print_block_list(fp3->inode->file_blocks));
    

    create_file(1023, filename, drive, &files_table); 
    printf("Aqui deberia imprimir los bloques del index 0: %s\n", print_block_list(files_table.table[0].file_ref->inode->file_blocks));
    printf("Aqui deberia imprimir el tamano en index 0 bien: %d END\n", files_table.table[0].file_ref->inode->size);

    printf("\n");
    print_files(&files_table);
    printf("\n");



    clean_up_storage(drive);


    

    return 0;
}
