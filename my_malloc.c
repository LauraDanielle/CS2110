#include "my_malloc.h"

/* You *MUST* use this macro when calling my_sbrk to allocate the
 * appropriate size. Failure to do so may result in an incorrect
 * grading!
 */
#define SBRK_SIZE 2048

/* If you want to use debugging printouts, it is HIGHLY recommended
 * to use this macro or something similar. If you produce output from
 * your code then you may receive a 20 point deduction. You have been
 * warned.
 */
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif
/*
* @author LAURA COX
*
*/

/* our freelist structure - this is where the current freelist of
 * blocks will be maintained. failure to maintain the list inside
 * of this structure will result in no credit, as the grader will
 * expect it to be maintained here.
 * Technically this should be declared static for the same reasons
 * as above, but DO NOT CHANGE the way this structure is declared
 * or it will break the autograder.
 */
metadata_t* freelist;

/* sorFreelistSize(int size, metadata_t* ptr)
 * Helper function that sorts freelist in size order
 * int size: size of block being freed and/or replaced in block
 * metadata_t* ptr: block to be placed back in free list
 *
 */
void sortFreelistSize(int size, metadata_t* ptr) {


        metadata_t* curr = freelist;
        if (curr == NULL) {
            freelist = ptr;
            freelist->size = size;

        }

        if (size < SBRK_SIZE && freelist->next != NULL) {
            metadata_t* buddy = mergeBuddy(ptr);
            if (buddy != NULL) {
                ptr = buddy;
                size = buddy->size;

            }
        }

        ptr->in_use = 0;

        while (curr != NULL) { 

            if (curr->prev == NULL && size < (curr->size)) { //block first in list
                freelist = ptr;
                ptr->next = curr;
                curr->prev = ptr;
                return;
            } else if (size < (curr->size)) { //block middle in list
               ptr->prev = curr->prev; //insert freed block into this
               ptr->next = curr;
               curr->prev->next = ptr;
               curr->prev = ptr;
               return;
            } else if (curr->next == NULL) { //block last in list
                ptr->prev = curr;
                curr->next = ptr;
                return;
            }
            curr = curr->next;

        }
    
}

/* Helper function mergeBuddy(metadata_t* buddy)
* @param metadata_t* buddy: block to check for merge
* @return metadata_t* changed block
* loops through current freelist to see if there is a buddy for the pointer
*
*/
metadata_t* mergeBuddy(metadata_t* buddy) {
    metadata_t* curr = freelist;

    metadata_t* currAddr;
    metadata_t* buddyAddr = (metadata_t*)((char*)buddy + buddy->size);

    metadata_t* tempNode = NULL;
    metadata_t* left = NULL;
    metadata_t* right = NULL;

    while (curr != NULL) {
        currAddr = (metadata_t*)((char*)curr + curr->size); 
        //check the left block to see if it can be coalesed 
        //block comes before buddy
        if (leftBuddy(currAddr, buddy)) {
            left = curr;
            //check if adding buddy and size gives the next address
        } else if (rightBuddy(curr, buddyAddr)) {
            right = curr;
        }
        
        curr = curr->next;
    }
    if (right != NULL && left != NULL) { // 3 cases for coalescing. Only checked after interating through list
        tempNode = left;
        tempNode->size += (right->size + buddy->size);
        removeNode(left, tempNode);
        removeNode(right, tempNode);

    } else if (left != NULL) {
        tempNode = left;
        tempNode->size += buddy->size;
        removeNode(left, tempNode);

    } else if (right != NULL) {
        tempNode = buddy;
        tempNode->size += right->size;

        removeNode(right, tempNode);

    }

    return tempNode;

}
//checks if there is a left buddy
int leftBuddy(metadata_t* currentNode, metadata_t* buddy) {
    if (buddy == currentNode) {
        return 1;
    }
    return 0;
}
//checks if there is a right buddy
int rightBuddy(metadata_t* currentNode, metadata_t* buddyNext) {
    if (currentNode == buddyNext) {
        return 1;
    }

    return 0;
}
//simply removes a node from the list (only used in mergeBuddy)
void removeNode(metadata_t* block, metadata_t* buddy) {

    if (block->prev != NULL) {
        block->prev->next = block->next; 
    } else {
        freelist = buddy;
        buddy->next = block->next;
    }
    if (block->next != NULL) {
        block->next->prev = block->prev;
    }

}
/* Helper function sortFreelistAddr(size_t size, metadata_t* ptr)
* int size: requested size from user, metadata_t* address: block to  place into freelist 
* calls merge when necessary, sorts pointer by address size
*
*/
void sortFreelistAddr(int size, metadata_t* ptr) 
{

    metadata_t* curr = freelist;
    if (curr == NULL) {
        freelist = ptr;
        freelist->size = size;

    }
    if (size < SBRK_SIZE && freelist->next != NULL) { //will not merge two 2048 blocks
        metadata_t* buddy = mergeBuddy(ptr);
        if (buddy != NULL) { //if buddy is not null, something was merged
            ptr = buddy;
            size = buddy->size;

        }
    }
    ptr->in_use = 0;
    while (curr != NULL) {
        if (curr->prev == NULL && (ptr < curr)) { //check if address is less than current address and first in list
            freelist = ptr;
            ptr->next = curr;
            curr->prev = ptr;
            return;
        } else if (ptr < curr) { // block in middle
            ptr->prev = curr->prev; 
            ptr->next = curr;
            curr->prev->next = ptr;
            curr->prev = ptr;
            return; 
        } else if (curr->next == NULL) { //block is last
            ptr->prev = curr;
            curr->next = ptr;
            return;
        }
        curr = curr->next;
    }

}

/* Helper function my_malloc_remove(int size, metadata_t* address, int order)
* int size: requested size from user, metadata_t* address: block to remove from freelist, 
* int order:which freelist sorting method to use (order = 0, size otherwise address)
* Contains case of if you can split the block, or if you must return the whole block because it would not have enough data without split
*/
void* my_malloc_remove(int size, metadata_t* address, int order) {
    void* tempPointer = NULL;

    if (address != NULL) {
        address->in_use = 1;

        if (address->size > (size + sizeof(metadata_t) + 1)) { //if block is big enough split it
            
            metadata_t* freeBlock = (metadata_t*)((char*) address + size); //set address of where block to be split
            freeBlock->size = address->size - (size); //decrease size of free memory block

            address->size = size; //change size of removed block

            if (address->prev != NULL) {
                address->prev->next = address->next; //previous block is equal to next block
                if (order == 0) {
                    sortFreelistSize(freeBlock->size, freeBlock); //put free block back into freelist
                } else {
                    sortFreelistAddr(freeBlock->size, freeBlock);
                }
            } else {
                freelist = freeBlock; //otherwise at front of the list so replace with freeblock
                freeBlock->next = address->next;
            }

            if (address->next != NULL) {
                address->next->prev = address->prev;
                if (order == 0) {
                    sortFreelistSize(freeBlock->size, freeBlock); //put free block into freelist after removing orinigal
                } else {
                    sortFreelistAddr(freeBlock->size, freeBlock);
                }
            } 

        } else {
            if (address->prev != NULL) {
                address->prev->next = address->next;
            } else {
                freelist = address->next;
            }//removing current block from free list
            if (address->next != NULL) {
                address->next->prev = address->prev;
            }

        }


        tempPointer = (void*) ((char*)address + sizeof(metadata_t)); //return a pointer without metadata
    }

    return tempPointer;

}
/* Helper function enoughSpaceSize(int size)
* int size: requested size from user
* Finds smallest block or returns NULL if there is not a big enough block
*/
metadata_t* enoughSpaceSize(int size) 
{
    metadata_t* curr = freelist;

    while (curr != NULL) {

        if (curr->size >= size) { //check if block is bigger than requested size
            return curr; //return that block
        }

        curr = curr->next;
    }

    return NULL;

}
/* Helper function enoughSpaceAddr(int size)
* int size: requested size from user
* Keeps track of current smallest block
* if there is a smaller block that fits criteria, it will choose that block instead
*/
metadata_t* enoughSpaceAddr(int size) 
{
    metadata_t* curr = freelist;
    int tempSize = SBRK_SIZE + 10; //no node can be bigger than this
    metadata_t* tempNode = NULL;
    while (curr != NULL) {
        if (curr->size >= size) { //check if block is bigger than requested size
            if (tempSize > curr->size) { //check if running total of block is bigger than requested size
                tempNode = curr; //update temporary block
                tempSize = curr->size;
            }
        }
        curr = curr->next;
    }

    return tempNode;

}

void* my_malloc_size_order(size_t size)
{
  
    int requestedSize = size + sizeof(metadata_t);


    if (requestedSize > SBRK_SIZE) 
    { //bigger than biggest block of free data there would be
        ERRNO = SINGLE_REQUEST_TOO_LARGE;

        return NULL;
    }
    if (freelist == NULL) 
    { //check if freelist is empty
        freelist = my_sbrk(SBRK_SIZE);
        if (freelist == NULL) {
            ERRNO = OUT_OF_MEMORY;
            return NULL;
        } else {
            ERRNO = NO_ERROR;
            freelist->size = SBRK_SIZE;
        }
    
    }

    metadata_t* spaceCheck = enoughSpaceSize(requestedSize);

    if (spaceCheck != NULL) 
    { //function that checks if there is enough space
        ERRNO = NO_ERROR;
        return my_malloc_remove(requestedSize, spaceCheck, 0);

    } else {
    //ERRNO = SINGLE_REQUEST_TOO_LARGE;
        metadata_t* address = my_sbrk(SBRK_SIZE);

        if (address == NULL) 
        {
            ERRNO = SINGLE_REQUEST_TOO_LARGE;
            return NULL;
        } else {
            ERRNO = OUT_OF_MEMORY;
            //function to insert more memory in freelist
            address->size = SBRK_SIZE;
            sortFreelistSize(address->size, address);
            spaceCheck = enoughSpaceSize(requestedSize);
            return my_malloc_remove(requestedSize, spaceCheck, 0); //add another 2048 block
        }

    }
}


void* my_malloc_addr_order(size_t size)
{
  
    int requestedSize = size + sizeof(metadata_t);


    if (requestedSize > SBRK_SIZE) 
    { //bigger than biggest block of free data there would be
        ERRNO = SINGLE_REQUEST_TOO_LARGE;
        return NULL;
    }
    if (freelist == NULL) 
    { //check if freelist is empty
        freelist = my_sbrk(SBRK_SIZE);
        if (freelist == NULL) {
            ERRNO = OUT_OF_MEMORY;
            return NULL;
        } else {
            ERRNO = NO_ERROR;
            freelist->size = SBRK_SIZE;
        }
    }
    metadata_t* spaceCheck = enoughSpaceAddr(requestedSize);
      
    if (spaceCheck != NULL) 
    { //function that checks if there is enough space
        ERRNO = NO_ERROR;
        return my_malloc_remove(requestedSize, spaceCheck, 1);

    } else {
    //ERRNO = SINGLE_REQUEST_TOO_LARGE;
        metadata_t* address = my_sbrk(SBRK_SIZE);
        if (address == NULL) 
        {
            ERRNO = SINGLE_REQUEST_TOO_LARGE;
            return NULL;
        } else {
            ERRNO = OUT_OF_MEMORY;
            //function to insert more memory in freelist
            address->size = SBRK_SIZE;
            sortFreelistAddr(address->size, address); //add new SBRK block to freelist
            spaceCheck = enoughSpaceAddr(requestedSize); //now check if there is enough size
            return my_malloc_remove(requestedSize, spaceCheck, 1); 
        }

    }
}


void my_free_size_order(void* ptr)
{

    ptr = ((metadata_t*)ptr) - 1; //start pointer at metadata

    
    sortFreelistSize(((metadata_t*)ptr)->size,(metadata_t*) ptr);

}

void my_free_addr_order(void* ptr)
{
    ptr = ((metadata_t*)ptr) - 1; //start pointer at metadata

    sortFreelistAddr(((metadata_t*)ptr)->size,(metadata_t*) ptr);
    
}
