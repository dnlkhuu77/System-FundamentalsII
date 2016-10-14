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
    cr_assert(headofx->header.padding_size == 12);

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
    cr_assert(headofx->header.padding_size == 12);

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

    cr_assert(freelist_head == (void*) x - 8);
    sf_free_header *headofx = (sf_free_header*)((char*)x-8);
    sf_footer *footofx = (sf_footer*) ((char*)((char*)headofx + (headofx->header.block_size<<4)) - 8);

    cr_assert(headofx->header.alloc == 0);
    cr_assert(headofx->header.block_size<<4 == 64);
    cr_assert(headofx->header.padding_size == 12);

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