#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sfmm.h"

int counter = 0;

/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */

sf_free_header* freelist_head = NULL;


void *sf_malloc(size_t size){ //size is int, double (you need to add header+footer+padding)
	printf("Size of whatever is: %d\n", size); //first test should be int
	size_t asize; 
	char *bp;

	if(size == 0)
		return NULL;

	if(freelist_head == NULL){ //at the very start
		freelist_head = (sf_free_header*) sf_sbrk(1); //request more memory
		freelist_head->next = NULL;
		freelist_head->prev = NULL;
		counter++;
	}

	if(counter == 4){
		//set errno to ENOMEM
		return NULL;
	}

	if(size <= 8) //adjust the block size to include overhead and alignment reqs.
		asize = 2 * 8; //8 is dsize (double word size 8 bytes)
	else
		asize = 8 * ((size + 8 + 7) / 8);

	if((bp = find_fit(asize)) != NULL){ //find a free block with asize size

		sf_header s1 = (sf_header*) bp; //set the header of the free list to allocated
		s1->alloc = 0x1;
		s1->block_size = size;
		s1->padding_size = 64 - size - 16;

		//go the the footer
		s1 = s1 + asize - 1;
		s1->alloc = 0x1;
		s1->block_size = size;
		s1->padding_size = 64 - size - 16;

		freelist_head = s1 + 1;
	
  		return s1;
 	}

 	sf_sbrk(1);
 	counter++;

 	if(counter == 4)
 		return NULL;
}

//find the biggest block of memory that will fit the allocated block
static void *find_fit(size_t asize){ //pg 856
	void* bp;

	for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
		if(!GET_ALLOC(HRDP(bp)) && (asize <= GET_SIZE(HRDP(bp)))){
			return bp;
		}
	}
	return NULL;
}

void sf_free(void *ptr){

}

void *sf_realloc(void *ptr, size_t size){
  return NULL;
}

int sf_info(info* meminfo){
  return -1;
}
