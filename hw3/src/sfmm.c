#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sfmm.h"
#include "sfmm2.h"

int counter = 0;

/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */

sf_free_header* freelist_head = NULL;


void *sf_malloc(size_t size){
	int stuff = 0;
	size_t asize;
	char *bp;
	sf_header* s1;
	sf_footer* s2;
	int padd = 0;
	int test_pad = size;
	char* justin = NULL;

	if(size == 0)
		return NULL;

	if(freelist_head == NULL){ //at the very start
		freelist_head = (sf_free_header*) sf_sbrk(1); //request more memory

		s1 = (sf_header*) freelist_head;
		s1->alloc = 0x0;
		s1->block_size = 4096;
		s1->padding_size = 0;

		freelist_head->header = *s1;
		freelist_head->next = NULL;
		freelist_head->prev = NULL;
		
		justin = (char*) freelist_head;
		justin = justin + 4096 - 8; //go to the footer

		s2 = (sf_footer*) justin;
		s2->alloc = 0x0;
		s2->block_size = 4096;

		//freelist_head is unaffected while we set the freelist_head to one big free block
		
		counter++;
	}

	if(counter == 4){
		errno = ENOMEM;
		return (void*) -1;
	}

	asize = size;

	//if((bp = find_fit(asize)) != NULL){ //find a free block with asize size
		bp = (char*) freelist_head;
		s1 = (sf_header*) bp; //set the header of the free list to allocated
		s1->alloc = 0x1;
		test_pad = asize;
		while(test_pad % 16 != 0){
			test_pad++;
			padd++;
		}

		s1->padding_size = padd;

		stuff = 16 + padd + asize;
		s1->block_size = stuff >> 4;

		//go the the footer
		s2 = (sf_footer*)((char*) (bp + asize + padd + 8));
		s2->alloc = 0x1;
		s2->block_size = stuff >> 4;
		
		bp = (void*) bp + 8; //move to the payload
		sf_varprint(bp);

		//edit the free block
		freelist_head = (sf_free_header*)((char*) (bp + asize + padd + 8));
		//the size of the free block left
  		return bp;
 //	}

 //	sf_sbrk(1);
 //	counter++;
}

void sf_free(void *ptr){
	sf_header* s1;
	sf_footer* s2;
	void* cal_ptr;
	int size;

	cal_ptr = ptr - 8; //go to the header from the payload
	s1 = (sf_header*) cal_ptr; //get the header out of the block
	size = s1->block_size << 4;
	
	s1->alloc = 0x0;

	s2 = (sf_footer*)((char*) (ptr + size - 16)); //ptr is at payload
	s2->alloc = 0x0;
	freelist_head = ptr;

}

void *sf_realloc(void *ptr, size_t size){
  return NULL;
}

int sf_info(info* meminfo){
  return -1;
}
