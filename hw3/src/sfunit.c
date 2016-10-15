#include <criterion/criterion.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfmm.h"
#include <errno.h>

/**
 *  HERE ARE OUR TEST CASES NOT ALL SHOULD BE GIVEN STUDENTS
 *  REMINDER MAX ALLOCATIONS MAY NOT EXCEED 4 * 4096 or 16384 or 128KB
 */

Test(sf_memsuite, Malloc_an_Integer, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(sizeof(int));
    *x = 4;
    cr_assert(*x == 4, "Failed to properly sf_malloc space for an integer!");
}

Test(sf_memsuite, Free_block_check_header_footer_values, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *pointer = sf_malloc(sizeof(short));
    sf_free(pointer);
    pointer = pointer - 8;
    sf_header *sfHeader = (sf_header *) pointer;
    cr_assert(sfHeader->alloc == 0, "Alloc bit in header is not 0!\n");
    sf_footer *sfFooter = (sf_footer *) (pointer - 8 + (sfHeader->block_size << 4));
    cr_assert(sfFooter->alloc == 0, "Alloc bit in the footer is not 0!\n");
}

Test(sf_memsuite, PaddingSize_Check_char, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *pointer = sf_malloc(sizeof(char));
    pointer = pointer - 8;
    sf_header *sfHeader = (sf_header *) pointer;
    cr_assert(sfHeader->padding_size == 15, "Header padding size is incorrect for malloc of a single char!\n");
}

Test(sf_memsuite, Check_next_prev_pointers_of_free_block_at_head_of_list, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(4);
    memset(x, 0, 4);
    cr_assert(freelist_head->next == NULL);
    cr_assert(freelist_head->prev == NULL);
}

Test(sf_memsuite, Coalesce_no_coalescing, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(4);
    int *y = sf_malloc(4);
    memset(y, 0xFF, 4);
    sf_free(x);
    cr_assert(freelist_head == (void*)x-8);
    sf_free_header *headofx = (sf_free_header*)((char*)x-8);
    sf_footer *footofx = (sf_footer*) ((char*)((char*)headofx + (headofx->header.block_size<<4)) - 8);
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
    cr_assert(headofx->header.block_size<<4 == 32);
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);
    cr_assert(footofx->block_size<<4 == 32);
}

/*
//############################################
// STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
// DO NOT DELETE THESE COMMENTS
//############################################
*/


Test(sf_memsuite, Mutliple_Malloc, .init = sf_mem_init, .fini = sf_mem_fini){
    int *w = sf_malloc(sizeof(int));
    int *x = sf_malloc(sizeof(int));
    long *y = sf_malloc(sizeof(long));
    int *z = sf_malloc(sizeof(int));

    memset(w, 0xFF, sizeof(int));
    memset(z, 0xFF, sizeof(int));
    memset(y, 0xFF, sizeof(long));
    sf_free(x);

    cr_assert(freelist_head == (void*) x - 8);
    sf_free_header *headofx = (sf_free_header*)((char*)x-8);
    sf_footer *footofx = (sf_footer*) ((char*)((char*)headofx + (headofx->header.block_size<<4)) - 8);

    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
    cr_assert(headofx->header.block_size<<4 == 32);
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);
    cr_assert(footofx->block_size << 4 == 32);
}


Test(sf_memsuite, Mutliple_COL, .init = sf_mem_init, .fini = sf_mem_fini){
    int *w = sf_malloc(sizeof(int));
    int *x = sf_malloc(sizeof(int));
    int *y = sf_malloc(sizeof(int));
    long *z = sf_malloc(sizeof(long));

    memset(w, 0xFF, sizeof(int));
    memset(z, 0xFF, sizeof(long));
    sf_free(y);
    sf_free(x);

    printf("ADDRESS OF FREELIST HEAD: %p\n", freelist_head);
    cr_assert(freelist_head == (void*) x - 8);
    sf_free_header *headofx = (sf_free_header*)((char*)x-8);
    sf_footer *footofx = (sf_footer*) ((char*)((char*)headofx + (headofx->header.block_size<<4)) - 8);

    cr_assert(headofx->header.alloc == 0);
    cr_assert(headofx->header.block_size<<4 == 64);
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);
    cr_assert(footofx->block_size << 4 == 64);
}

Test(sf_memsuite, MIXED_M_F, .init = sf_mem_init, .fini = sf_mem_fini){
    int *x = sf_malloc(sizeof(int)); //will be freed
    int *y = sf_malloc(sizeof(int));
    sf_free(x);
    int *z = sf_malloc(sizeof(int)); //replace x

    memset(y, 0xFF, sizeof(int));

    sf_free_header *headofz = (sf_free_header*)((char*)z-8);
    sf_footer *footofz = (sf_footer*) ((char*)((char*)headofz + (headofz->header.block_size<<4)) - 8);

    printf("ADDRESS OF FREELIST: %p\n", (void*) ((char*)headofz + (headofz->header.block_size<<4) + 
        (((sf_free_header*) ((char*)z-8))->header.block_size<<4)));
    printf("ADRESS OF ACTUAL FREELIST: %p\n", freelist_head);
    cr_assert(freelist_head == (void*) ((char*)headofz + (headofz->header.block_size<<4) + 
        (((sf_free_header*) ((char*)z-8))->header.block_size<<4)));
    
    cr_assert(headofz->header.alloc == 1);
    cr_assert(headofz->header.block_size<<4 == 32);
    cr_assert(headofz->header.padding_size == 12);

    cr_assert(footofz->alloc == 1);
    cr_assert(footofz->block_size << 4 == 32);
}

Test(sf_memsuite, NULL_FREE, .init = sf_mem_init, .fini = sf_mem_fini){
    //int *x = sf_malloc(0) a ;
    int* x = NULL;
    sf_free(x);

    printf("ERRNO PRINTS: %s\n", strerror(errno));
    cr_assert(strcmp(strerror(errno), "Invalid argument") == 0);
}

Test(sf_memsuite, MALLOC_FREE_ERROR, .init = sf_mem_init, .fini = sf_mem_fini){
    int *x = sf_malloc(0);
    sf_free(x);

    cr_assert(strcmp(strerror(errno), "Invalid argument") == 0);
}

Test(sf_memsuite, SPLINTER, .init = sf_mem_init, .fini = sf_mem_fini){
    int *x = sf_malloc(31);
    double* y = sf_malloc(sizeof(double));

    sf_free(x);
    int *z = sf_malloc(4);
    sf_free_header *headofy = (sf_free_header*)((void*)y - 8);

    sf_footer *footofy = (sf_footer*)((char*)(((char*)headofy) + (headofy->header.block_size << 4)) - 8);

    cr_assert(freelist_head == (void*)footofy + 8);

    sf_free_header* headofz = (sf_free_header*)((char*)z - 8);
    sf_footer* footofz = (sf_footer*)((char*)((char*)headofz + (headofz->header.block_size << 4)) - 8);

    cr_assert(headofz->header.alloc == 1);
    cr_assert(headofz->header.block_size << 4 == 48);
    cr_assert(headofz->header.padding_size == 12);

    cr_assert(footofz == (void*)headofy - 8);
}

Test(sf_memsuite, MALLOC_OVER_FOUR, .init = sf_mem_init, .fini = sf_mem_fini){
    int* a = sf_malloc(16384);
    cr_assert(a == NULL);
}

Test(sf_memsuite, MALLOC_WHOLE, .init = sf_mem_init, .fini = sf_mem_fini){
    int* a = sf_malloc(4016);
    int* b = sf_malloc(20);

    sf_free_header* headofa = (sf_free_header*)((void*)a - 8);
    sf_footer* footofa = (sf_footer*)((void*)((void*)headofa + (headofa->header.block_size << 4)) - 8);

    sf_free_header* headofb = (sf_free_header*)((void*)b - 8);
    sf_footer* footofb = (sf_footer*)((void*)((void*)headofb + (headofb->header.block_size << 4)) - 8);

    cr_assert(headofa->header.block_size << 4 == 4032);
    cr_assert(footofa->block_size << 4 == 4032);
    cr_assert(headofb->header.block_size << 4 == 64);
    cr_assert(footofb->block_size << 4 == 64);

    cr_assert(freelist_head == NULL);
}

Test(sf_memsuite, MALLOC_FOUR, .init = sf_mem_init, .fini = sf_mem_fini){
    int* x = sf_malloc((4*4096) - 16);

    sf_free_header* headofx = (sf_free_header*)((void*)x - 8);
    sf_footer* footofx = (sf_footer*)((void*)((void*)headofx + (headofx->header.block_size << 4)) - 8);

    cr_assert(freelist_head == NULL);
    cr_assert(headofx->header.block_size << 4 == (4096*4));
    cr_assert(footofx->block_size << 4 == (4096*4));
    cr_assert(headofx->header.alloc == 1);
    cr_assert(footofx->alloc == 1);
}

Test(sf_memsuite, REALLOC_NO_SPLINTER_NEXTBLOCKFREE, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(112); //used to be 112
 
    sf_free_header* headofx = (sf_free_header*)((char*) x - 8);
    sf_footer* footofx = (sf_footer*)((char*)((char*)headofx + (headofx->header.block_size << 4)) - 8);
 
    cr_assert(headofx->header.block_size << 4 == 128);
    cr_assert(headofx->header.alloc == 1);
    cr_assert(headofx->header.padding_size == 0);
 
    cr_assert(footofx->block_size << 4 == 128);
    cr_assert(footofx->alloc == 1);
 
    cr_assert(freelist_head == (void*)footofx + 8);
 
    sf_realloc(x, 4);
 
    cr_assert(headofx->header.block_size << 4 == 32);
    cr_assert(headofx->header.alloc == 1);
    cr_assert(headofx->header.padding_size == 12);
 
    cr_assert(footofx->alloc == 0);
 
    sf_free_header* freehead = (sf_free_header*)((char*)headofx + (headofx->header.block_size << 4));

    cr_assert(freelist_head == freehead);
    cr_assert(freelist_head->header.block_size << 4 == 4064); // 4096 - 128(MALLOC) + 96(REALLOC);
}

Test(sf_memsuite, REALLOC_NOSPLINTER_NEXTBLOCKALLOC, .init = sf_mem_init, .fini = sf_mem_fini) {
    // REALLOC, NO SPLINTER; NEXT BLOCK ALLOC
    int *x = sf_malloc(112);   // 128 BYTES
    int *y = sf_malloc(4);     // 32 BYTES
 
    sf_free_header* headofx = (sf_free_header*)((char*) x - 8);
    sf_footer* footofx = (sf_footer*)((char*)((char*)headofx + (headofx->header.block_size << 4)) - 8);
 
    sf_free_header* headofy = (sf_free_header*)((char*) y - 8);
    sf_footer* footofy = (sf_footer*)((char*)((char*)headofy + (headofy->header.block_size << 4)) - 8);
 
    cr_assert(headofx->header.block_size << 4 == 128);
    cr_assert(headofx->header.alloc == 1);
    cr_assert(headofx->header.padding_size == 0);
 
    cr_assert(footofx->block_size << 4 == 128);
    cr_assert(footofx->alloc == 1);
 
    cr_assert(headofy->header.block_size << 4 == 32);
    cr_assert(headofy->header.alloc == 1);
    cr_assert(headofy->header.padding_size == 12);
 
    cr_assert(freelist_head == (void*)footofy + 8);
 
    sf_realloc(x, 4);      // 32 BYTES
 
    cr_assert(headofx->header.block_size << 4 == 32);
    cr_assert(headofx->header.alloc == 1);
    cr_assert(headofx->header.padding_size == 12);
 
    cr_assert(footofx->alloc == 0);
 
    sf_free_header* freehead = (sf_free_header*)((char*)headofx + (headofx->header.block_size << 4));
 
    cr_assert(freelist_head == freehead);
    cr_assert(freelist_head->header.block_size << 4 == 96); // 96(REALLOC);
 
}
