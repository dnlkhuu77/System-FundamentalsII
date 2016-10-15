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
int counter, sbrk_count, d_flag = 0;
uint64_t d_end = 4096;
int top_flag = 0;
size_t internal, alloc_size, external, allocations, frees = 0;

void *sf_malloc(size_t size){
	d_flag = 0; //only set when you malloc not at the end
	if(size <= 0 || size > ((4096 * 4) - 16)){
		errno = ENOMEM;
		return NULL;
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
		return NULL;
	}

	if(alloc_size < size){ ///if there is no free space left
		if(freelist_head == NULL){
			freelist_head = (sf_free_header*) sf_sbrk(1);
			freelist_head = ((void*) freelist_head) - 4096;

			if(top_flag == 0){
				top_ptr = freelist_head;
				top_flag = 1;
			}

			alloc_size = alloc_size + 4096;
			external = external + 4080;

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
				alloc_size = alloc_size + 4096;
				external = external + 4096;

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
				alloc_size = alloc_size + 4096;
				external = external + 4096;

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
				alloc_size = alloc_size + 4096;
				external = external + 4096;

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
			external = external + 4096;
			oHead = ((void*) oHead) - 4096;

			if(sbrk_count >= 1){ 
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
			}
			if(sbrk_count >= 2){
				oHead = (sf_free_header*) sf_sbrk(2);
				alloc_size = alloc_size + 4096;
				external = external + 4096;
				oHead = ((void*) oHead) - (4096 * 2);

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
				external = external + 4096;
				oHead = ((void*) oHead) - (4096 * 3);

				oHead = ((void*) oHead) - d_end;
				oHead->header.alloc = 0;
				oHead->header.block_size = alloc_size >> 4; //it already increase by 4096
				oHead->header.padding_size = 0;

				freelist_foot = (sf_footer*) (((void*) sf_sbrk(0)) - 8); //request more memory
				freelist_foot->alloc = 0x0;
				freelist_foot->block_size = alloc_size >> 4;
			}
		}
	}

	sf_free_header* find_space = freelist_head; 
	while(find_space != NULL){
		if((find_space->header.block_size << 4) >= size){ //32 >= 32
			sf_free_header* find_next = NULL;
			sf_free_header* find_prev = NULL; 
			sf_header* s1 = (void*) find_space;
			sf_footer* s2 = (void*) find_space;

			uint64_t padd = 0;
			uint64_t test_pad = size;
			int stuff = 0;

			while(test_pad % 16 != 0){
				test_pad++;
				padd++;
			}

			if(((find_space->header.block_size << 4) - (size + padd+ 16)) < 32){ //SPLINTERS!
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

				s1->alloc = 0x1;
				s1->padding_size = padd;

				stuff = (find_space->header.block_size << 4);
				s1->block_size = (stuff >> 4);

				//go the the footer
				s2 = (sf_footer*)((void*) (s2) + (find_space->header.block_size << 4) - 8);
				s2->alloc = 0x1;
				s2->block_size = (stuff >> 4);
				alloc_size = alloc_size - stuff;
				external = external - stuff + 16;
				internal = internal + stuff - size;;

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

				s1->alloc = 0x1;
				s1->padding_size = padd;

				stuff = 16 + padd + size;
				s1->block_size = (stuff >> 4);
				alloc_size = alloc_size - stuff;
				external = external - stuff + 16;
				internal = internal + stuff - size;

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
	return NULL;
}

void sf_free(void *ptr){ 
	if(ptr == NULL || strcmp(strerror(errno), "Cannot allocate memory") == 0){
		errno = EINVAL;
	}
	else{
		ptr = ptr - 8; //go to the header
		sf_header* cal_ptr = (sf_header*) ptr;
		if(cal_ptr->alloc != 1 || (cal_ptr->block_size << 4) <= 0){
			errno = EFAULT;
		}
		else{
			coalesce(ptr);
		}
	}
}

void coalesce(void* ptr){ 
	frees++;
	sf_free_header* cal_ptr = (sf_free_header*) ptr; //cal_ptr holds the header address of the affected block
	int size = cal_ptr->header.block_size << 4; //size holds the size of the affected block 
	sf_footer* foot = ((void*) cal_ptr) + size - 8; //foot gets the footer

	sf_header* start = (sf_header*)(((void*) cal_ptr) - 8);
	sf_footer* end = (sf_footer*)(((void*) foot) + 8);

	if(cal_ptr->header.alloc == 0 && end->alloc == 0){
		//merge [*F][F] (like case 2)
		printf("SBRK_FREE\n");
		sf_free_header* oldHead = cal_ptr; //oldHead will be the first block || cal_ptr will be the second block

		cal_ptr = ((void*) cal_ptr) + size;
		int64_t b_move = (cal_ptr->header.block_size) << 4; //b_move is the size of the second block

		foot = (sf_footer*)((void*)cal_ptr + b_move - 8); //the foot
		foot->block_size = (b_move + size) >> 4;
		oldHead->header.block_size = (b_move + size) >> 4;

		freelist_head = oldHead;
		freelist_foot = foot;
	}
	else if((sf_header*)cal_ptr == (sf_header*) top_ptr && end->alloc == 1){
		printf("CASE 1\n");
		alloc_size = alloc_size + size;
		external = external + size - 16;
		internal = internal - 16 - cal_ptr->header.padding_size;
		//[M*][M][F]
		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.padding_size = 0;
		cal_ptr->header.block_size = size >> 4;
		foot->alloc = 0x0;
		foot->block_size = size >> 4;

		sf_free_header* temp = freelist_head;
		freelist_head = cal_ptr;
		freelist_head->next = temp;
		temp->prev = freelist_head;

	}
	else if(start->alloc == 1 && end->alloc == 0){
		//[M][M*][F]
		printf("CASE 3\n");
		alloc_size = alloc_size + size;
		external = external + size - 16;
		internal = internal - 16 - cal_ptr->header.padding_size;
		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.padding_size = 0;

		sf_free_header* head_check = (void*) foot;
		head_check = ((void*) head_check) + 8;

		foot = ((void*) foot) + (head_check->header.block_size << 4);
		foot->alloc = 0;
		foot->block_size = ((head_check->header.block_size << 4) + (cal_ptr->header.block_size << 4)) >> 4;
		cal_ptr->header.block_size = foot->block_size; //update the header
		freelist_head = ((void*) cal_ptr);
		freelist_head->next = head_check->next; 
		freelist_foot = foot;
		d_end = d_end + (size);
	}
	else if(start->alloc ==0 && end->alloc == 1){
		//[F][M*][M]
		printf("CASE 2\n");
		alloc_size = alloc_size + size;
		external = external + size -16;
		internal = internal - 16 - cal_ptr->header.padding_size;
		cal_ptr->header.alloc = 0x0;
		cal_ptr->header.padding_size = 0;

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
		external = external + size - 16;
		internal = internal - 16 - cal_ptr->header.padding_size;
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
		d_end = d_end + size + b_moveheader;
	}
	else if(start->alloc == 1 && end->alloc == 1){
		//[M][M*][M]
		printf("CASE 5\n");
		alloc_size = alloc_size + size;
		external = external + size - 16;
		internal = internal - 16 - cal_ptr->header.padding_size;
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
	//shrink, enlarge, search the free list, sbrk (memcopy)
	int convert_flag = 0;
	sf_footer* oldFoot;
	sf_footer* newFoot;
	if(ptr == NULL || size <= 0){
		errno = EINVAL;
		return NULL;
	}
	ptr = ptr - 8; //go to the header
	sf_free_header* cal_ptr = (sf_free_header*) ptr;
	if(cal_ptr->header.alloc != 1 || (cal_ptr->header.block_size << 4) <= 0){
			errno = EFAULT;
			return NULL;
	}

	size_t actual_size = cal_ptr->header.block_size << 4;
	sf_free_header* next_block = (void*)cal_ptr + actual_size;
	int splinter_check = 0;

	if(actual_size == size) //PLEASE INVLUDE THE PADDING!
		return (ptr + 8);
	else if(actual_size > size)
		convert_flag = -1; //shrink
	else if(actual_size < size)
		convert_flag = 1; //expand

	uint64_t padd = 0;
	uint64_t test_pad = size;

	while(test_pad % 16 != 0){
		test_pad++;
		padd++;
	}

	if(convert_flag == -1){ //shrink
		splinter_check = actual_size - (size + 16);
		if(splinter_check < 32){ //splinter
			cal_ptr->header.padding_size = padd;
			return (void*)cal_ptr + 8;
		}else{ //no splinters
			//check the new block of ptr to see if we need to coalesce the free block we created
			if(next_block->header.alloc == 1){
				//the next block is alloacted
				oldFoot = (sf_footer*)((void*)cal_ptr + actual_size - 8);
				oldFoot->alloc = 0;
				oldFoot->block_size = (-1) >> 4;

				newFoot = (sf_footer*)((void*)cal_ptr + size + padd + 16 - 8);
				newFoot->alloc = 1;
				newFoot->block_size = (size + 16 + padd) >> 4;
				cal_ptr->header.block_size = (size + 16 + padd) >> 4;
				cal_ptr->header.padding_size = padd;
				cal_ptr->header.alloc = 1;

				//move the freelist_head
				freelist_head = (sf_free_header*)((void*)newFoot + 8);
				freelist_head->header.alloc = 0;
				freelist_head->header.block_size = ((actual_size) - size - padd - 16) >> 4;
				printf("SIZE OF FREELIST: %d\n", freelist_head->header.block_size);
				return (void*) cal_ptr + 8;

			}else{
				//the next block is free (coalesce)
				oldFoot = (sf_footer*)((void*)cal_ptr + actual_size - 8);
				oldFoot->block_size = (-1) >> 4;
				oldFoot->alloc = 0x0;

				newFoot = (sf_footer*)((void*)cal_ptr + size + padd + 16 - 8);
				newFoot->alloc = 1;
				newFoot->block_size = (size + padd + 16) >> 4;
				cal_ptr->header.block_size = (size + padd + 16) >> 4;
				cal_ptr->header.padding_size = padd;
				cal_ptr->header.alloc = 1;

				sf_free_header* newBlock = ((void*)newFoot + 8);
				newBlock->header.alloc = 0;
				newBlock->header.block_size = ((actual_size) - size - padd - 16) >> 4;
				coalesce((void*)newFoot + 8);

				freelist_head = (sf_free_header*)((void*)newFoot + 8);
				return (void*)cal_ptr + 8;
			}
		}

	}/*else if(convert_flag == 1){
		if(next_block->header.alloc == 0){ //next to free block
			int more_space = (next_block->header.block_size << 4);

			if(more_space < (actual_size - size)){
				//if the free block next to it is not enough for the expansion
				//move to another space (possible sbrk from malloc)
				void* sf_malloc(size);
				memcopy()
			}

			splinter_check = actual_size - (size + 16);
			if(splinter_check < 32){
				//splinter
			}
			else if(splinter_check >=32){
				//no splinter
			}
		}else{ //next to allocated block
			//move to another space (possible sbrk from malloc)

		}
	}*/


  	return NULL;
}

int sf_info(info* meminfo){
	if(meminfo == NULL)
		return -1;

	if(internal >= 0 && external >= 0 && allocations >= 0 && frees >= 0){
		meminfo->internal = internal;
		meminfo->external = external;
		meminfo->allocations = allocations;
		meminfo->frees = frees;
		return 0;
	}
  return -1;
}
