/*mm1.c As mentioned in question we have used Best Fit strategy to allocate free blocks
 * and two-sided coalescing
 * we have implemented two doubly linked list one for free blocks(free list) and other for allocated blocks(explicit free list)
 * and we have kept free list in sorted order of addresses so that two-sided coalescing can be implemented in O(1) time
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Mind_Palace",
    /* First member's full name */
    "Pranjal Saini",
    /* First member's email address */
    "pranjalsaini@cse.iitb.ac.in",
    /* Second member's full name (leave blank if none) */
    "Harsh Peswani",
    /* Second member's email address (leave blank if none) */
    "harshpeswani@cse.iitb.ac.in"
};



/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 
 * mm_init - initialize the malloc package.
 */

void *init_mem_sbrk_break = NULL;

struct header_t{
	int size;
    int free;
    struct header_t *next;
    struct header_t *prev;
};

struct header_t *head=NULL;
struct header_t *head_allocated=NULL;
void* prev_value;
int i=0;

int mm_init(void)
{
	
	
	
	head_allocated=NULL;
	head=NULL;
	
	//This function is called every time before each test run of the trace.
	//It should reset the entire state of your malloc or the consecutive trace runs will give wrong answer.	
	

	/* 
	 * This function should initialize and reset any data structures used to represent the starting state(empty heap)
	 * 
	 * This function will be called multiple time in the driver code "mdriver.c"
	 */
	
    return 0;		//Returns 0 on successfull initialization.
}

//---------------------------------------------------------------------------------------------------------------
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{	
	/* 
	 * This function should keep track of the allocated memory blocks.
	 * The block allocation should minimize the number of holes (chucks of unusable memory) in the heap memory.
	 * The previously freed memory blocks should be reused.
	 * If no appropriate free block is available then the increase the heap  size using 'mem_sbrk(size)'.
	 * Try to keep the heap size as small as possible.
	 */
	
	if(size <= 0){		// Invalid request size
		return NULL;
	}
	size = ((size+7)/8)*8;		//size alligned to 8 bytes


	//Best Fit starts here

	int min_size=__INT_MAX__;
	struct header_t *temp=head;
	struct header_t *best_fit=NULL;
	while(temp!=NULL){
		if(temp->size >= size){
			int tmp=temp->size-size;
			if(tmp<min_size){
				min_size=tmp;
				best_fit=temp;
			}
		}
		temp=temp->next;
	}
	
	//Best Fit ends here
	int prev_size=0;
	if(best_fit!=NULL){
		prev_size=best_fit->size;
	}
	//Allocation Starts Here
	if(best_fit==NULL){
		
		best_fit=(struct header_t*)mem_sbrk(size+sizeof(struct header_t));
		
		best_fit->prev=NULL;
		best_fit->next=NULL;
		prev_size=size;
	}
	best_fit->size=size;
	//best_fit->free=0;
	void *new=(void *)best_fit+sizeof(struct header_t)+size;
	
	//int new_size=(int)new;
	struct header_t *new1=NULL;
	if(prev_size-size > sizeof(struct header_t)){
		/*struct header_t *temp=head;
		while(temp!=NULL){
			printf("\n%p\n",temp);
			temp=temp->next;
		}*/
		
		new1=(struct header_t*)new;
		new1->size=prev_size-size-sizeof(struct header_t);
		//new1->free=1;
		new1->next=best_fit->next;
		new1->prev=best_fit->prev;
        if(head==best_fit){
            head=new1;
        }
		if(best_fit->prev!=NULL){
			best_fit->prev->next=new1;
		}
		if(best_fit->next!=NULL){
			best_fit->next->prev=new1;
		}
        
        best_fit->next=NULL;
	    best_fit->prev=NULL;
		/*printf("\n----------\n");
		temp=head;
		while(temp!=NULL){
			printf("\n%p\n",temp);
			temp=temp->next;
		}
		printf("\n###############\n");*/
	}else{
		
		best_fit->size=prev_size;
		
        if(head==best_fit){
            head=head->next;
			if(head!=NULL)
           		head->prev=NULL;
            best_fit->next=NULL;
            best_fit->prev=NULL;
        }else{
            if(best_fit->prev!=NULL){
				
                best_fit->prev->next=best_fit->next;
				
			}
            if(best_fit->next!=NULL)
                best_fit->next->prev=best_fit->prev;
            best_fit->prev=NULL;
            best_fit->next=NULL;
        }
		
	}
	if(head==NULL){
		head=new1;
	}
	

	if(head_allocated==NULL){
		head_allocated=best_fit;
	}
	else{
		head_allocated->prev=best_fit;
		best_fit->next=head_allocated;
		head_allocated=best_fit;
		head_allocated->prev=NULL;
	}
	/*printf("----------\n");
		temp=head;
		while(temp!=NULL){
			printf("%p %d\n",temp,temp->size);
			temp=temp->next;
		}
		printf("###############\n");*/
	return (void *)((void*)best_fit+sizeof(struct header_t));		//mem_sbrk() is wrapper function for the sbrk() system call. 
								//Please use mem_sbrk() instead of sbrk() otherwise the evaluation results 
								//may give wrong results
}

void mm_free(void *ptr)
{
	/* 
	 * Searches the previously allocated node for memory block with base address ptr.
	 * 
	 * It should also perform coalesceing on both ends i.e. if the consecutive memory blocks are 
	 * free(not allocated) then they should be combined into a single block.
	 * 
	 * It should also keep track of all the free memory blocks.
	 * If the freed block is at the end of the heap then you can also decrease the heap size 
	 * using 'mem_sbrk(-size)'.
	 */
	
	struct header_t *temp=head_allocated;
	int flag=0;
	while(temp!=NULL){
		if((void *)(temp)+sizeof(struct header_t) == ptr ){
			flag=1;
			break;
		}
		temp=temp->next;
	}
	if(flag==0){
		return;
	}
	
	//temp->free=1;
	if (head_allocated == NULL || temp == NULL) 
	return; 

	if (head_allocated == temp) 
		head_allocated = temp->next; 

	if (temp->next != NULL) 
		temp->next->prev = temp->prev; 

	if (temp->prev != NULL) 
		temp->prev->next = temp->next;
	
	struct header_t* current=NULL; 
  
 
   if(head == NULL) {
       temp->next = NULL;
       temp->prev = NULL;
	   head=temp;
	}
	else {
       current = head;
       if((long)head> (long)(temp)){
           temp->next=head;
           head->prev=temp;
           head=temp;
       }else{
            while(current->next != NULL && (long)current->next < (long)temp) {
                current = current->next;
            }
            temp->prev = current;
            temp->next = current->next;

            if(current->next != NULL) {
                current->next->prev = temp;
            }
            current->next = temp;
       } 
	}
	head->prev=NULL;
	if(temp!=NULL && temp->next!=NULL){
		if((void *)temp+temp->size+sizeof(struct header_t)==(void *)temp->next){
			struct header_t *ntmp=temp->next;
			temp->size=temp->size+sizeof(struct header_t)+temp->next->size;
			temp->next=temp->next->next;
			if(ntmp->next!=NULL)
				ntmp->next->prev=temp;
			ntmp->next=NULL;
			ntmp->prev=NULL;
		}
	}
	if(temp!=NULL && temp->prev!=NULL){
		if((void *)(temp->prev)+temp->prev->size+sizeof(struct header_t)==(void *)temp){
			
			struct header_t *ntmp=temp->prev;
			int ad=temp->size;
			
			temp->prev->size=ad+(temp->prev->size)+sizeof(struct header_t);
			
			if(temp!=NULL && temp->prev !=NULL){
				temp->prev->next=temp->next;
			}
			if(temp->next!=NULL && temp->prev!=NULL){
				temp->next->prev=temp->prev;
				temp->next=NULL;
			}
			temp->next=NULL;
			temp->prev=NULL;
			temp=ntmp;
		}
	}
	/*printf("----------\n");
		temp=head;
		while(temp!=NULL){
			printf("%p %d\n",temp,temp->size);
			temp=temp->next;
		}
		printf("###############\n");*/

    
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{	
	size = ((size+7)/8)*8; //8-byte alignement	
	
	if(ptr == NULL){			//memory was not previously allocated
		return mm_malloc(size);
	}
	
	if(size == 0){				//new size is zero
		mm_free(ptr);
		return NULL;
	}
	struct header_t *temp=head_allocated;
	int flag=0;
	while(temp!=NULL){
		if((void *)(temp)+sizeof(struct header_t) == ptr ){
			flag=1;
			break;
		}
		temp=temp->next;
	}
	if(flag==0){
		return NULL;
	}
	struct header_t *asli_ptr=  (struct header_t*)(ptr-sizeof(struct header_t));
	if(asli_ptr->size == size){
		return ptr;
	}
	else if(asli_ptr->size > size){
		if(asli_ptr->size-size > sizeof(struct header_t)){
			int prev_size=asli_ptr->size;
			asli_ptr->size=size;
			struct header_t *tmp= (struct header_t*)(ptr+size);
			//tmp->free=0;
			tmp->size=prev_size-size-sizeof(struct header_t);
			tmp->next=asli_ptr->next;
			tmp->prev=asli_ptr;
			asli_ptr->next=tmp;
			mm_free((void *)tmp+sizeof(struct header_t));
			return ptr;
		}else{
			//struct header_t *free=(struct header_t*) ptr+size;
			//int free_size=asli_ptr->size-size;
			//void *free_one=(void*)free + free_size;

			return ptr;
		}
	}else{
		/*struct headert_t* check=(struct header_t *)(ptr+asli_ptr->size);
		struct header_t* tmp=head;
		while(tmp!=NULL){
			if((void*)tmp==(void*)check){
				if((asli_ptr->size+tmp->size+sizeof(struct header_t))>=size){
					int needed=size-asli_ptr->size-sizeof(struct header_t);
					int bacha=tmp->size-needed;
					if(bacha>sizeof(struct header_t)){

					}else{
						if(tmp->prev)
					}

				}
				break;
			}
		}*/
		void* naya=(void *)mm_malloc(size);
		void * chk=memcpy(naya,ptr,asli_ptr->size);
		if(chk==NULL){
			printf("\nError in copying please try again latter\n");
			return ptr;
		}
		mm_free(ptr);
		return naya;

	}
	


	/*
	 * This function should also copy the content of the previous memory block into the new block.
	 * You can use 'memcpy()' for this purpose.
	 * 
	 * The data structures corresponding to free memory blocks and allocated memory 
	 * blocks should also be updated.
	*/
	
}














