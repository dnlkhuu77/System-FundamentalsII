#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "sfmm.h"
#include "sfmm2.h"

int free_size = 0;

/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */

sf_free_header* freelist_head = NULL;
sf_free_header* top_ptr = NULL;
sf_free_header* bottom_ptr = NULL;
sf_footer* freelist_foot = NULL;
int counter, sbrk_count = 0;
int alloc_size, d_flag = 0;
uint64_t d_end = 4096;
int internal, allocations, frees = 0;


void *sf_malloc(size_t size){
	d_flag = 0; //only set when you malloc not at the end
	if(size <= 0 || size > ((4096 * 4) - 16)){
		errno = ENOMEM;
		return (void*) -1;
	}
	else if(size <= 4080 && alloc_size < size){
		sbrk_count = 1;
		counter++;
	}
	else if(size <= ((4096 * 2) - 16) && alloc_size < size){
		sbrk_count = 2;
		counter = counter + 2;
	}
	else if(size <= ((4096 * 3) - 16) && alloc_size < size){
		sbrk_count = 3;
		counter = counter + 3;
	}
	else if(size <= ((4096 * 4) - 16) && alloc_size < size){
		sbrk_count = 4;
		counter = counter + 4;
	}

	if(counter > 4){
		errno = ENOMEM;
		return (void*) -1;
	}

	if(alloc_size < size){ ///if there is no free space left
		if(freelist_head == NULL){
			freelist_head = (sf_free_header*) sf_sbrk(1); //POSSIBLE ERROR
			freelist_head = ((void*) freelist_head) - 4096;

			top_ptr = freelist_head;
			//printf("Address of freelist_head: %p\n", freelist_head);
			alloc_size = alloc_size + 4096;

			freelist_head->header.alloc = 0x0;
			freelist_head->header.block_size = alloc_size >> 4;
			freelist_head->header.padding_size = 0;

			freelist_foot = (sf_footer*) ((void*) freelist_head + alloc_size - 8); //request more memory
			freelist_foot->alloc = 0x0;
			freelist_foot->block_size = alloc_size >> 4;

			if(sbrk_count >= 2){
				freelist_head = (sf_free_header*) sf_sbrk(2);
				freelist_head = ((void*) freelist_head) - (4096 * 2);

				top_ptr = freelist_head;
				//printf("Address of freelist_head: %p\n", freelist_head);
				alloc_size = alloc_size + 4096 + 4096;

				freelist_head->header.alloc = 0x0;
				freelist_head->header.block_size = alloc_size >> 4;
				freelist_head->header.padding_size = 0;

				freelist_foot = (sf_footer*) (((void*) freelist_head) + (alloc_size - 8)); //request more memory
				freelist_foot->alloc = 0x0;
				freelist_foot->block_size = alloc_size >> 4;
			}
			if(sbrk_count >= 3){
				freelist_head = (sf_free_header*) sf_sbrk(3);
				freelist_head = ((void*) freelist_head) - (4096 * 3);

				top_ptr = freelist_head;
				//printf("Address of freelist_head: %p\n", freelist_head);
				alloc_size = alloc_size + 4096 + 4096 + 4096;

				freelist_head->header.alloc = 0x0;
				freelist_head->header.block_size = alloc_size >> 4;
				freelist_head->header.padding_size = 0;

				freelist_foot = (sf_footer*) (((void*) freelist_head) + (alloc_size - 8)); //request more memory
				freelist_foot->alloc = 0x0;
				freelist_foot->block_size = alloc_size >> 4;
			}
			if(sbrk_count >= 4){
				freelist_head = (sf_free_header*) sf_sbrk(4);
				freelist_head = ((void*) freelist_head) - (4096 * 4);

				top_ptr = freelist_head;
				//printf("Address of freelist_head: %p\n", freelist_head);
				alloc_size = alloc_size + 4096 + 4096 + 4096 + 4096;

				freelist_head->header.alloc = 0x0;
				freelist_head->header.block_size = alloc_size >> 4;
				freelist_head->header.padding_size = 0;

				freelist_foot = (sf_footer*) (((void*) freelist_head) + (alloc_size - 8)); //request more memory
				freelist_foot->alloc = 0x0;
				freelist_foot->block_size = alloc_size >> 4;
			}


			freelist_head->prev = NULL;
			freelist_head->next = NULL;
		}
		else{ //the freelist_header is not NULL [MixtureofMandF][FreeLIST]
			sf_free_header* oHead = NULL;
			oHead = (sf_free_header*) sf_sbrk(1); //make more space
			alloc_size = alloc_size + 4096;
			oHead = ((void*) oHead) - 4096; //NEW

			if(sbrk_count >= 1){ //you add 1 sbrk 

				sf_free_header* find_space = ((void*) freelist_head) - 8; //go to the previous block to set ptrs
				if(find_space->header.alloc == 0){ //if the previous block is free, coalesce
					oHead = ((void*) oHead) - d_end;
					oHead->header.alloc = 0;
					oHead->header.block_size = alloc_size >> 4; //it already increase by 4096
					oHead->header.padding_size = 0;

					freelist_foot = (sf_footer*) (((void*) oHead) + (d_end + 4096 - 8)); //request more memory
					freelist_foot->alloc = 0x0;
					freelist_foot->block_size = alloc_size >> 4;
				}
				else{
					oHead->header.alloc = 0;
					oHead->header.block_size = alloc_size >> 4; //it already increase by 4096
					oHead->header.padding_size = 0;

					freelist_foot = (sf_footer*) (((void*) oHead) + (4096 - 8)); //request more memory
					freelist_foot->alloc = 0x0;
					freelist_foot->block_size = alloc_size >> 4;
				}
 
			} //you add 2 sbrk
			if(sbrk_count >= 2){
				oHead = (sf_free_header*) sf_sbrk(2);
				alloc_size = alloc_size + 4096;
				oHead = ((void*) oHead) - (4096 * 2);

				//sf_footer* find_footspace = ((sf_footer*) find_space);
				//uint64_t freesize = find_footspace->block_size << 4; //get the block size of the free
				oHead = ((void*) oHead) - d_end;
				oHead->header.alloc = 0;
				oHead->header.block_size = alloc_size >> 4; //it already increase by 4096
				oHead->header.padding_size = 0;

				freelist_foot = (sf_footer*) (((void*) sf_sbrk(0)) - 8); //request more memory
				freelist_foot->alloc = 0x0;
				freelist_foot->block_size = alloc_size >> 4;

			}
			if(sbrk_count >= 3){
				oHead = (sf_free_header*) sf_sbrk(3);
				alloc_size = alloc_size + 4096; //already incremented twice
				oHead = ((void*) oHead) - (4096 * 3);

				//sf_footer* find_footspace = ((sf_footer*) find_space); //go to the previous block
				//uint64_t freesize = find_footspace->block_size << 4; //get the block size of the free
				oHead = ((void*) oHead) - d_end;
				oHead->header.alloc = 0;
				oHead->header.block_size = alloc_size >> 4; //it already increase by 4096
				oHead->header.padding_size = 0;

				freelist_foot = (sf_footer*) (((void*) sf_sbrk(0)) - 8); //request more memory
				freelist_foot->alloc = 0x0;
				freelist_foot->block_size = alloc_size >> 4;
			}
			//printf("ADDRESS OF THE FREELIST_FOOT: %p\n", sf_sbrk(0));
			//freelist_head = oHead;
			//freelist_head->next = NULL;
			//freelist_head->prev = oldHead;
		}
	}

	sf_free_header* find_space = freelist_head; 
	while(find_space != NULL){
		if((find_space->header.block_size << 4) >= size){ //32 >= 32
			sf_free_header* find_next = NULL; //cursorNext
			sf_free_header* find_prev = NULL; //cursorPrev
			sf_header* s1 = (void*) find_space;
			sf_footer* s2 = (void*) find_space;

			if(((find_space->header.block_size << 4) - (size + 16)) < 32){ //SPLINTERS!
				if(find_space->next == NULL && find_space->prev == NULL){
					freelist_head = NULL;
					d_flag = 1;
				}
				else if(find_space->next != NULL && find_space->prev == NULL){ //HEAD
					find_next = find_space->next;
					find_next->prev = find_space->prev;
					find_next = freelist_head;
				}
				else if(find_space->next != NULL && find_space->prev != NULL){ //MIDDLE
					find_next = find_space->next;
					find_prev = find_space->prev;
					find_next->prev = find_prev;
					find_prev->next = find_next;
				}
				else if(find_space->next == NULL && find_space->prev != NULL){ //TAIL
					find_prev = find_space->prev;
					find_prev->next = find_space->next;
					d_flag = 1;
				}

				uint64_t padd = 0;
				uint64_t test_pad = size;
				int stuff = 0;

				s1->alloc = 0x1;

				while(test_pad % 16 != 0){
					test_pad++;
					padd++;
				}

				s1->padding_size = padd;

				stuff = 16 + padd + size;
				s1->block_size = (stuff >> 4);

				//go the the footer
				s2 = (sf_footer*)((void*) (s2) + size + padd + 8);
				s2->alloc = 0x1;
				s2->block_size = (stuff >> 4);
				alloc_size = alloc_size - stuff;
				internal = internal + stuff;

				if(alloc_size == 0)
					freelist_head = NULL;

				if(d_flag == 1 && counter == 1){
					d_end = d_end - stuff;
					d_flag = 0;
				}

				sf_free_header* oHead = NULL;
				if(freelist_head != NULL){ 
					oHead = freelist_head;
					freelist_head = ((void*) s1) + stuff;

					while(freelist_head->header.alloc == 1){
						freelist_head = (sf_free_header*)(((void*) freelist_head) + (freelist_head->header.block_size << 4));
					}

					oHead->prev = freelist_head;
					freelist_head->next = oHead;
					freelist_head->prev = NULL;
				}
				else{
					freelist_head = ((void*)s1) + stuff;
					freelist_head->next = NULL;
					freelist_head->prev = NULL;
				}
			}
			else{ //NON SPLINTERS
				if(find_space->next == NULL && find_space->prev == NULL){
					freelist_head = NULL;
					d_flag = 1;
				}
				else if(find_space->next != NULL && find_space->prev == NULL){ //HEAD
					find_next = find_space->next;
					find_next->prev = find_space->prev;
					find_next = freelist_head;
				}
				else if(find_space->next != NULL && find_space->prev != NULL){ //MIDDLE OF THE BLOCK
					find_next = find_space->next;
					find_prev = find_space->prev;
					find_next->prev = find_prev;
					find_prev->next = find_next;
				}
				else if(find_space->next == NULL && find_space->prev != NULL){ //TAIL
					find_prev = find_space->prev;
					find_prev->next = find_space->next;
					d_flag = 1;
				}

				uint64_t b_size = (find_space->header.block_size) << 4;
				uint64_t padd = 0;
				uint64_t test_pad = size;
				int stuff = 0;

				s1->alloc = 0x1;

				while(test_pad % 16 != 0){
					test_pad++;
					padd++;
				}

				s1->padding_size = padd;

				stuff = 16 + padd + size;
				s1->block_size = (stuff >> 4);
				alloc_size = alloc_size - stuff;
				internal = internal + stuff;

				//go the the footer
				s2 = (sf_footer*)((void*) (s2) + size + padd + 8);
				s2->alloc = 0x1;
				s2->block_size = (stuff >> 4);

				find_space = ((void*) s2) + 8; //move the pointer to the end of the footer

				if(d_flag == 1 && counter == 1){
					d_end = d_end - stuff;
					d_flag = 0;
				}

				sf_free_header* oHead = NULL;
				if(freelist_head != NULL){ //the updated free block becomes the header
					oHead = freelist_head;
					freelist_head = ((void*) s1) + stuff;

					while(freelist_head->header.alloc == 1){
						freelist_head = (sf_free_header*)(((void*) freelist_head) + (freelist_head->header.block_size << 4));
					}

					oHead->prev = freelist_head;
					freelist_head->next = oHead;
					freelist_head->prev = NULL;
				}
				else{
					freelist_head = ((void*)s1) + stuff;
					freelist_head->next = NULL;
					freelist_head->prev = NULL;
				}

				freelist_head->header.alloc = 0x0;
				freelist_head->header.block_size = (b_size - (s1->block_size << 4)) >> 4;

				sf_footer* cFooter = ((void*) freelist_head) + ((freelist_head->header.block_size << 4) - 8);
				cFooter->alloc = 0x0;
				cFooter->block_size = freelist_head->header.block_size;
			}
			allocations++;
			return ((void*) s1) + 8;
		}
		find_space = find_space->next;
	}
	return (void*) -1;
}

void sf_free(void *ptr){ 
	if(ptr == NULL || strcmp(strerror(errno), "Cannot allocate memory") == 0){
		errno = EINVAL;
	}
	else{
		ptr = ptr - 8; //go to the header
		sf_header* cal_ptr = (sf_header*) ptr;
		if(cal_ptr->alloc == 0 || (cal_ptr->block_size << 4) <= 0 || (void*)cal_ptr > (void*)sf_sbrk(0)){
			errno = EFAULT;
		}
		else{
			coalesce(ptr);
		}
	}

}

void coalesce(void* ptr){
	//return an error if you try to free null, middle of allication block, 
	frees++;
	sf_free_header* cal_ptr = (sf_free_header*) ptr; //cal_ptr holds the header address of the affected block
	uint64_t size = cal_ptr->header.block_size; //size holds the size of the affected block 
	size = size << 4;
	sf_footer* foot = ((void*) cal_ptr) + size - 8; //foot gets the footer

	sf_header* start = (sf_header*)(((void*) cal_ptr) - 8);
	sf_footer* end = (sf_footer*)(((void*) foot) + 8);

	if((sf_header*)cal_ptr == (sf_header*) top_ptr && end->alloc == 1){
		printf("CASE 1\n");
		alloc_size = alloc_size + size;
		internal = internal - size;
		//[M*][M][F]
		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.padding_size = 0;
		foot->alloc = 0x0;

		sf_free_header* temp = freelist_head;
		freelist_head = cal_ptr;
		freelist_head->next = temp;
		temp->prev = freelist_head;
		freelist_foot = foot;
	}
	else if(start->alloc == 1 && end->alloc == 0){
		//[M][M*][F]
		printf("CASE 3\n");
		alloc_size = alloc_size + size;
		internal = internal - size;
		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.padding_size = 0;
		foot->alloc = 0x0;

		sf_free_header* head_check = (void*) foot;
		head_check = ((void*) head_check) + 8;

		foot = ((void*) foot) + (head_check->header.block_size << 4);
		foot->block_size = ((head_check->header.block_size << 4) + (cal_ptr->header.block_size << 4)) >> 4;
		cal_ptr->header.block_size = foot->block_size;
		freelist_head = ((void*) cal_ptr);
		freelist_head->next = head_check->next;  
		freelist_foot = foot;
		d_end = d_end + (size);
	}
	else if(start->alloc ==0 && end->alloc == 1){
		//[F][M*][M]
		printf("CASE 2\n");
		alloc_size = alloc_size + size;
		internal = internal - size;
		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.padding_size = 0;
		foot->alloc = 0x0;

		cal_ptr = ((void*) cal_ptr) - 8;
		uint64_t b_move = (cal_ptr->header.block_size) << 4;
		cal_ptr = ((void*) cal_ptr) - (b_move - 8); //the -8 included the header

		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.block_size = (b_move + size) >> 4;

		foot->alloc = 0x0;
		foot->block_size = (b_move + size) >> 4;

		freelist_head = cal_ptr;
		freelist_foot = foot;
	}
	else if(start->alloc == 0 && end->alloc == 0){
		//[F][M*][F]
		printf("CASE 4\n");
		alloc_size = alloc_size + size;
		internal = internal - size;
		cal_ptr->header.alloc = 0x0;
		foot->alloc = 0x0;

		sf_free_header* head_check = cal_ptr;
		uint64_t b_move = (cal_ptr->header.block_size) << 4;
		head_check = (void*) head_check + b_move;

		cal_ptr = ((void*) cal_ptr) - 8;
		uint64_t b_moveheader = (cal_ptr->header.block_size) << 4;
		cal_ptr = ((void*) cal_ptr) - (b_moveheader - 8);

		foot = ((void*) foot) + 8;
		uint64_t b_movefooter = (foot->block_size) << 4;
		foot = ((void*) foot) + (b_movefooter - 8);

		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.block_size = (b_movefooter + b_moveheader + size) >> 4;
		cal_ptr->header.padding_size = 0;

		foot->alloc = 0x0;
		foot->block_size = (b_movefooter + b_moveheader + size) >> 4;

		freelist_head = cal_ptr;
		freelist_head->next = head_check->next;
		freelist_foot = foot;
		d_end = d_end + size + b_moveheader; //the foot is already freed and is part of d_end
	}
	else if(start->alloc == 1 && end->alloc == 1){
		//[M][M*][M]
		printf("CASE 5\n");
		alloc_size = alloc_size + size;
		internal = internal - size;
		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.padding_size = 0;
		foot->alloc = 0x0;

		sf_free_header* temp = freelist_head;
		freelist_head = cal_ptr;
		cal_ptr->next = temp;
		temp->prev = cal_ptr;
		freelist_foot = foot;
	}
}

void *sf_realloc(void *ptr, size_t size){
	//shrink, enlarge, search the free list, sbrk
	//use memset
	if(ptr == NULL || size == 0){
		errno = EINVAL;
		return (void*) -1;
	}

	ptr = ptr - 8; //go to the header
	//sf_free_header* cal_ptr = (sf_free_header*) ptr;


  	return NULL;
}

int sf_info(info* meminfo){
	if(internal < 0 || alloc_size < 0 || allocations < 0 || frees < 0){
		meminfo->internal = 0;
		meminfo->external = alloc_size;
		meminfo->allocations = 0;
		meminfo->frees = 0;
		return 0;
	}
  return -1;
}
