#include "main.h"

int main()
{
    


    block block1 = {0, 511, 0, 1};
    block block2 = {511, 1023, 1, 1};
    block block3 = {1023, 1535, 2, 1};
    block block4 = {0, 511, 0, 3};
    block block5 = {511, 1023, 1, 4};
    block block6 = {1023, 1535, 2, 5};
    block block7 = {1023, 1535, 2, 5};
    
    storage *file_system = (storage*)malloc(sizeof(storage));
    block blocks[7] = {block1,block2,block3,block4,block5,block6,block7};

    initialize_storage(file_system, 7, blocks);
    bool stat = file_system->bad_blocks[0];
    int stat2 = file_system->usage_registry[0];



    block_allocated_list test_list = {NULL, NULL, 0};

    add_node(&test_list, block1.block_id);
    add_node(&test_list, block2.block_id);
    add_node(&test_list, block3.block_id);

    printf("Block list is : %s \n total size of block is: %d", print_block_list(&test_list), test_list.size);
    printf("head is: %d and tail is %d\n", test_list.head->block_id, test_list.tail->block_id);


    //file2; 
    
    block_allocated_list test_list_2 = {NULL, NULL, 0};

    add_node(&test_list_2, block1.block_id);
    add_node(&test_list_2, block2.block_id);
    add_node(&test_list_2, block3.block_id);

    printf("Block list is : %s \n total size of block is: %d", print_block_list(&test_list_2), test_list_2.size);
    printf("head is: %d and tail is %d\n", test_list_2.head->block_id, test_list_2.tail->block_id);

    
    clean_up_storage(file_system);


    
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
    */
    return 0;
}
