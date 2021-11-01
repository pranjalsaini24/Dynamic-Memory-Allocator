/*
 * mm2.c
 * in this implementation we have used balanced binary search tree(AVL) tree to maintain free list and allocated list
 * so that insertion deletion and searching takes O(logn) time,we have used worst-fit and two-sided coalescing strategy
 * we have used worst-fit so that there will be small number of holes and less external fragmentation and coalescing will
 * merge adjacent block thus increasing block size. 
 * 
 * If there is no free block of required size during allocation and if there is a free block right before break point(mem_sbrk(0))
 * so we will use that free block and call mem_sbrk for remaining amount of memory so this save us from requesting more memory
 * from mem_sbrk.
 * 
 * In realloc if there is request to increase the size so if the free block is available right next to the previously allocated
 * block we have used that free block to optimise utilazation  
 * 
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
    int height;
    struct header_t *left;
    struct header_t *right;
};

struct header_t *root_free=NULL;
struct header_t *root_allocated=NULL;
struct header_t *worst_fit=NULL;
struct header_t *aakhri=NULL;

void* prev_value;
int i=0;
int find=0;
int counter=0;
struct header_t *left_coal=NULL;

int mm_init(void)
{
	
	

	root_free=NULL;
	root_allocated=NULL;
	worst_fit=NULL;
	aakhri=NULL;
	find=0;
	left_coal=NULL;
	
	
	//This function is called every time before each test run of the trace.
	//It should reset the entire state of your malloc or the consecutive trace runs will give wrong answer.	
	

	/* 
	 * This function should initialize and reset any data structures used to represent the starting state(empty heap)
	 * 
	 * This function will be called multiple time in the driver code "mdriver.c"
	 */
	
    return 0;		//Returns 0 on successfull initialization.
}



struct header_t* minValueNode(struct header_t* node) 
{ 
    struct header_t* current = node; 

    while (current->left != NULL) 
        current = current->left; 
  
    return current; 
}
int max(int a, int b); 
  
int height(struct header_t *N) 
{ 
    if (N == NULL) 
        return 0; 
    return N->height; 
} 
  

int max(int a, int b) 
{ 
    return (a > b)? a : b; 
} 
 
struct header_t *rightRotate(struct header_t *y) 
{ 
    struct header_t *x = y->left; 
    struct header_t *T2 = x->right; 
  
    
    x->right = y; 
    y->left = T2; 
  
    
    y->height = max(height(y->left), height(y->right))+1; 
    x->height = max(height(x->left), height(x->right))+1; 
  
   
    return x; 
} 
  

struct header_t *leftRotate(struct header_t *x) 
{ 
    struct header_t *y = x->right; 
    struct header_t *T2 = y->left; 
  
     
    y->left = x; 
    x->right = T2; 
  
     
    x->height = max(height(x->left), height(x->right))+1; 
    y->height = max(height(y->left), height(y->right))+1; 
  
    
    return y; 
} 

//DELETE IT

struct header_t* delete_one(struct header_t* root, struct header_t* new,int check) 
{ 
  
  
    if (root == NULL) 
        return root; 
  
    if ( new < root ) 
        root->left = delete_one(root->left, new,check); 
  
    
    else if( new> root ) 
        root->right = delete_one(root->right,new,check); 
  

    else
    { 
        
        if( (root->left == NULL) || (root->right == NULL) ) 
        { 
            struct header_t *temp = root->left ? root->left : 
                                             root->right; 
  
            
            if (temp == NULL) 
            { 
                temp = root; 
				root->height=1;
				root->left=NULL;
				root->right=NULL;
                root = NULL; 
            } 
            else{
             root->left=NULL;
             root->right=NULL;
             root->height=1;  
             root = temp;
            } 
            
        } 
        else
        { 
           
            struct header_t* temp = minValueNode(root->right); 
  
            
            root->height=1;
            
            temp->left=root->left;
            temp->right=root->right;
            root->left=NULL;
            root->right=NULL;
            root=temp; 
  
            
        } 
    }
	 
  
   
    if (root == NULL) 
      return root; 
  
   
    
  
    return root; 
} 


struct header_t* deleteNode(struct header_t* root, struct header_t* new,int check) 
{ 
  
    if (root == NULL) 
        return root; 
  
   
    if ( new < root ) 
        root->left = deleteNode(root->left, new,check); 
  
   
    else if( new> root ) 
        root->right = deleteNode(root->right,new,check); 
  
    
    else
    { 
         
        if( (root->left == NULL) || (root->right == NULL) ) 
        { 
            struct header_t *temp = root->left ? root->left : 
                                             root->right; 
  
            
            if (temp == NULL) 
            { 
                temp = root; 
				root->height=1;
				root->left=NULL;
				root->right=NULL;
                root = NULL; 
            } 
            else{
             root->left=NULL;
             root->right=NULL;
             root->height=1;  
             root = temp;
            } 
            
        } 
        else
        { 
            struct header_t* temp = minValueNode(root->right); 
  
           
            root->height=1;
            if(check==0){
                root_free=delete_one(root_free,temp,check);

            }else{
                root_allocated=delete_one(root_allocated,temp,check);
            }
			
            temp->left=root->left;
            temp->right=root->right;
            root->left=NULL;
            root->right=NULL;
            root=temp; 
  
             
        } 
    } 
  
    
    if (root == NULL) 
      return root; 
  
    
   	root->height = 1 + max(height(root->left), 
                           height(root->right)); 

    int balance = getBalance(root); 
 
    if (balance > 1 && getBalance(root->left) >= 0) 
        return rightRotate(root); 
  
   
    if (balance > 1 && getBalance(root->left) < 0) 
    { 
        root->left =  leftRotate(root->left); 
        return rightRotate(root); 
    } 
  
    if (balance < -1 && getBalance(root->right) <= 0) 
        return leftRotate(root); 
  
    if (balance < -1 && getBalance(root->right) > 0) 
    { 
        root->right = rightRotate(root->right); 
        return leftRotate(root); 
    } 
  
    return root; 
} 

int getBalance(struct header_t *N) 
{ 
    if (N == NULL) 
        return 0; 
    return height(N->left) - height(N->right); 
} 
  
struct header_t* insert(struct header_t* node, struct header_t* new) 
{ 
	
    if (node == NULL) 
        return new; 
  
    if ((void *)new < (void *)node) 
        node->left  = insert(node->left,new); 
    else if ((void *)new > (void *)node) 
        node->right = insert(node->right,new); 
     
  
  
    node->height = 1 + max(height(node->left), 
                           height(node->right)); 
  
    
   int balance = getBalance(node); 
  
    if (balance > 1 && (void *)new <(void *) (node->left)) 
        return rightRotate(node); 
   
    if (balance < -1 && (void *)new > (void *)(node->right)) 
        return leftRotate(node); 
   
    if (balance > 1 && (void *)new> (void *)(node->left)) 
    { 
        node->left =  leftRotate((node->left)); 
        return rightRotate(node); 
    } 
 
    if (balance < -1 && new < (void *)(node->right)) 
    { 
        node->right = rightRotate((node->right)); 
        return leftRotate(node); 
    } 
  
    return node; 
} 
  
struct header_t * pred(struct header_t* node) 
{ 
    struct header_t* current = node; 
    while (current->right != NULL) 
        current = current->right; 
  
    return current; 
}                    
void worstfit(struct header_t *root,int min_size,int size) 
{ 
    if(root != NULL) 
    {  
        worstfit(root->left,min_size,size); 
        if(root->size >= size){
			int tmp=root->size-size;
			if(tmp>min_size){
				worst_fit=root;
				min_size=tmp;
			}
		}
		aakhri=root;
		worstfit(root->right,min_size,size); 
    } 
}


void search(struct header_t *root,struct header_t *to_find){
	if(root != NULL) 
    {  
       if(to_find<root){		
       	   search(root->left,to_find);
	   }else if(to_find>root){
            search(root->right,to_find);
	   }else{
		   find=1;
		   return;
	   }
	    
    } 

}

void inorder(struct header_t *root){
	if(root != NULL) 
    {  
       inorder(root->left);
	   printf("%u %d\n",root,root->size);
	   inorder(root->right);
	    
    } 

}



void search_left(struct header_t *root,struct header_t *to_find){
	if(root != NULL) 
    {  
       void * tmp=(void *)root+sizeof(struct header_t)+root->size;
	   if(tmp<(void *)to_find){
		   search_left(root->right,to_find);
	   }else if( tmp>(void *)to_find){
		   search_left(root->left,to_find);
	   }else{
		   left_coal=root;
		   return;
	   }
	    
    } 

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


	//Worst Fit starts here

	int min_size=-1;
	worst_fit=NULL;
	aakhri=NULL;
	worstfit(root_free,min_size,size);
	
	//Worst Fit ends here                  
	int prev_size=0;
	if(worst_fit!=NULL){
		prev_size=worst_fit->size;
	}
	struct header_t *temp1=aakhri;
	//Allocation Starts Here
	if(worst_fit==NULL){                                     
		
		if(temp1!=NULL){
			
			if((void *)temp1+sizeof(struct header_t)+temp1->size==mem_sbrk(0)){
				struct header_t* temp2=temp1;
				root_free=deleteNode(root_free,temp1,0);
				temp1->left=NULL;
				temp1->right=NULL;
				temp1->height=1;
				int mangna=size-temp1->size;
				mangna = ((mangna+7)/8)*8;
				
				worst_fit=(struct header_t *)mem_sbrk(mangna);
				
				worst_fit=temp1;
				worst_fit->left=NULL;
				worst_fit->right=NULL;
				worst_fit->height=1;
				worst_fit->size=size;
				
			}
			else{
				worst_fit=(struct header_t*)mem_sbrk(size+sizeof(struct header_t));
				worst_fit->left=NULL;
				worst_fit->right=NULL;
				worst_fit->height=1;
				worst_fit->size=size;
			}
		}else{
			
			worst_fit=(struct header_t*)mem_sbrk(size+sizeof(struct header_t));
			worst_fit->left=NULL;
			worst_fit->right=NULL;
			worst_fit->height=1;
			worst_fit->size=size;
		}
		worst_fit->left=NULL;
		worst_fit->right=NULL;
		worst_fit->height=1;
		prev_size=size;
	}
	worst_fit->size=size;
	//best_fit->free=0;
	void *new=(void *)worst_fit+sizeof(struct header_t)+size;
	
	//int new_size=(int)new;
	struct header_t *new1=NULL;
	if(prev_size-size > sizeof(struct header_t)){
		/*struct header_t *temp=head;
		while(temp!=NULL){
			printf("\n%p\n",temp);
			temp=temp->next;
		}*/
		root_free=deleteNode(root_free,worst_fit,0);
		worst_fit->left=NULL;
		worst_fit->right=NULL;
		worst_fit->height=1;
		worst_fit->size=size;
		new1=(struct header_t*)new;
		new1->size=prev_size-size-sizeof(struct header_t);
		//new1->free=1;
		new1->left=NULL;
		new1->right=NULL;
		new1->height=1;
		
		root_free=insert(root_free,new1);
		/*printf("\n----------\n");
		temp=head;
		while(temp!=NULL){
			printf("\n%p\n",temp);
			temp=temp->next;
		}
		printf("\n###############\n");*/
	}else{
		
		worst_fit->size=prev_size;
		root_free=deleteNode(root_free,worst_fit,0);
		
        worst_fit->left=NULL;
		worst_fit->right=NULL;
		worst_fit->height=1;
		
	}
	
	worst_fit->left=NULL;
	worst_fit->right=NULL;
	worst_fit->height=1;
	root_allocated=insert(root_allocated,worst_fit);
	/*printf("--------------------\n");
	inorder(root_free);
	printf("--------------------\n");*/

	/*printf("###########\n");
	inorder(root_allocated);
	printf("###########\n");*/
	return (void *)((void*)worst_fit+sizeof(struct header_t));		//mem_sbrk() is wrapper function for the sbrk() system call. 
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

	find=0;
	
	struct header_t *to_find=(struct header_t*)(ptr-sizeof(struct header_t));
	search(root_allocated,to_find);
	if(find==0){
		return;
	}
	
	root_allocated=deleteNode(root_allocated,to_find,1);
	
	to_find->left=NULL;
	to_find->right=NULL;
	to_find->height=1;


	root_free=insert(root_free,to_find);
	
	//RIGHT COALASING
	   struct header_t* succ=(struct header_t*)((void *)to_find +sizeof(struct header_t)+to_find->size);
        find=0;
        search(root_free,succ);
        if(find==1){
            to_find->size=to_find->size+sizeof(struct header_t)+succ->size;
            root_free=deleteNode(root_free,succ,0);
        }
    
	left_coal=NULL;
	search_left(root_free,to_find);
	if(left_coal!=NULL){
		left_coal->size=left_coal->size+sizeof(struct header_t)+to_find->size;
		root_free=deleteNode(root_free,to_find,0);
	}

	if(to_find->left!=NULL){
		struct header_t* prede=pred(to_find->left);
		if((void *)prede+sizeof(struct header_t)+prede->size==(void *)to_find){
			prede->size=prede->size+sizeof(struct header_t)+to_find->size;
			root_free=deleteNode(root_free,to_find,0);
		}
	}     
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
		return;
	}
	find=0;
	struct header_t *to_find=(struct header_t *)(ptr-sizeof(struct header_t));
	search(root_allocated,to_find);
	if(find==0){
		return;
	}
	if(to_find->size >= size){
		if(to_find->size-size >= sizeof(struct header_t)){
			int pr=to_find->size;
			to_find->size=size;
			void *new=(void *)to_find+sizeof(struct header_t)+size;
			struct header_t* new1=(struct header_t*)new;
			new1->height=1;
			new1->left=NULL;
			new1->right=NULL;
			new1->size=pr-size-sizeof(struct header_t);
			root_free=insert(root_free,new1);
			return ptr;
		}else{
			return ptr;
		}
	}else{
		
	    struct header_t* succ=(struct header_t*)((void *)to_find +sizeof(struct header_t)+to_find->size);
        find=0;
        search(root_free,succ);
		 if(find==1){
			int total_size=to_find->size+sizeof(struct header_t)+succ->size;
			if(size<=total_size){
				if(total_size-size>=sizeof(struct header_t)){
					root_free=deleteNode(root_free,succ,0);
					to_find->size=size;

					void *new=(void *)to_find+sizeof(struct header_t)+to_find->size;
					struct header_t* new1=(struct header_t*)new;
					new1->left=NULL;
					new1->right=NULL;
					new1->height=1;
					new1->size=total_size-size-sizeof(struct header_t);
					root_free=insert(root_free,new1);
					return ptr;
				}else{
					to_find->size=total_size;
					root_free=deleteNode(root_free,succ,0);
					return ptr;
				}
			}

		}
		int prev=to_find->size;
		
		void *naya=mm_malloc(size);
		void * chk=memcpy((void *)naya,ptr,prev);
		if(chk==NULL){
			exit(0);
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














