/*
 ******************************************************************************
 *                               mm.c                                         *
 *           64-bit struct-based implicit free list memory allocator          *
 *                      without coalesce functionality                        *
 *                 CSE 361: Introduction to Computer Systems                  *
 *                                                                            *
 *  ************************************************************************  *
 *                     insert your documentation here. :)                     *
 *           64-bit struct based explicit free list memory allocator with     *
 *          coalesce functionality built from base code described above ^^    *
 *  ************************************************************************  *
 *  ** ADVICE FOR STUDENTS. **                                                *
 *  Step 0: Please read the writeup!                                          *
 *  Step 1: Write your heap checker. Write. Heap. checker.                    *
 *  Step 2: Place your contracts / debugging assert statements.               *
 *  Good luck, and have fun!                                                  *
 *                                                                            *
 ******************************************************************************
 */

/* Do not change the following! */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stddef.h>

#include "mm.h"
#include "memlib.h"

#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* You can change anything from here onward */

/*
 * If DEBUG is defined, enable printing on dbg_printf and contracts.
 * Debugging macros, with names beginning "dbg_" are allowed.
 * You may not define any other macros having arguments.
 */
// #define DEBUG // uncomment this line to enable debugging

#ifdef DEBUG
/* When debugging is enabled, these form aliases to useful functions */
#define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_requires(...) assert(__VA_ARGS__)
#define dbg_assert(...) assert(__VA_ARGS__)
#define dbg_ensures(...) assert(__VA_ARGS__)
#else
/* When debugging is disnabled, no code gets generated for these */
#define dbg_printf(...)
#define dbg_requires(...)
#define dbg_assert(...)
#define dbg_ensures(...)
#endif

/* Basic constants */
typedef uint64_t word_t;
static const size_t wsize = sizeof(word_t);   // word and header size (bytes)
static const size_t dsize = 2*sizeof(word_t);       // double word size (bytes)
static const size_t min_block_size = 4*sizeof(word_t); // Minimum block size
static const size_t chunksize = (1 << 12);    // requires (chunksize % 16 == 0)

static const word_t alloc_mask = 0x1;
static const word_t size_mask = ~(word_t)0xF;





typedef struct block
{
    /* Header contains size + allocation flag */
    word_t header;
    /*
     * We don't know how big the payload will be.  Declaring it as an
     * array of size 0 allows computing its starting address using
     * pointer notation.
     */

    //  Struct modified to either carry prev and next pointers or payload
    // Struct is either free or allocated
    union{
        struct{
            struct block* prev;
            struct block* next;
        };
        char payload[0];
    };
    
    /*
     * We can't declare the footer as part of the struct, since its starting
     * position is unknown
     */
} block_t;


/* Global variables */
/* Pointer to first block in heap*/
static block_t *heap_start = NULL;
//Pointer to first block in free list
static block_t *free_listp = NULL;


bool mm_checkheap(int lineno);

/* Function prototypes for internal helper routines */
static block_t *extend_heap(size_t size);
static void place(block_t *block, size_t asize);
static block_t *find_fit(size_t asize);
static block_t *coalesce(block_t *block);

static size_t max(size_t x, size_t y);
static size_t round_up(size_t size, size_t n);
static word_t pack(size_t size, bool alloc);

static size_t extract_size(word_t header);
static size_t get_size(block_t *block);
static size_t get_payload_size(block_t *block);

static bool extract_alloc(word_t header);
static bool get_alloc(block_t *block);

static void write_header(block_t *block, size_t size, bool alloc);
static void write_footer(block_t *block, size_t size, bool alloc);

static block_t *payload_to_header(void *bp);
static void *header_to_payload(block_t *block);

static block_t *find_next(block_t *block);
static word_t *find_prev_footer(block_t *block);
static block_t *find_prev(block_t *block);

static void remove_free(block_t *block);
static void prepend(block_t *block);
static bool checkblock(block_t *block);

static void print_heap();
static void print_free();






/*
 * <what does mm_init do?>
 */
 //Creates the initial heap with free and empty header and footer
bool mm_init(void) 
{
    
    // Create the initial empty heap 
    word_t *start = (word_t *)(mem_sbrk(2*wsize));

    if (start == (void *)-1) 
    {
        return false;
    }

    start[0] = pack(0, true); // Prologue footer
    start[1] = pack(0, true); // Epilogue header
    // Heap starts with first "block header", currently the epilogue footer
    heap_start = (block_t *) &(start[1]);
    free_listp = NULL;

    // Extend the empty heap with a free block of chunksize bytes 
    if (extend_heap(chunksize) == NULL)
    {
        return false;
    }
    return true;
}

/*
 * <what does mmalloc do?>
 */
 //Searches free list for block of a size of at least "size"
 //If no block is found the heap is extended
 //If a block is found it is assigned the payload
void *malloc(size_t size) 
{
    // printf(" ** Malloc Started** \n");
    dbg_requires(mm_checkheap(__LINE__));
    size_t asize;      // Adjusted block size
    size_t extendsize; // Amount to extend heap if no fit is found
    block_t *block;
    void *bp = NULL;

    if (heap_start == NULL) // Initialize heap if it isn't initialized
    {
        mm_init();
    }

    if (size == 0) // Ignore spurious request
    {
        dbg_ensures(mm_checkheap(__LINE__));
        return bp;
    }

    // Adjust block size to include overhead and to meet alignment requirements
    asize = round_up(size + dsize, dsize);

    // Search the free list for a fit
    block = find_fit(asize);

    // If no fit is found, request more memory, and then and place the block
    if (block == NULL)
    {  
        extendsize = max(asize, chunksize);
        block = extend_heap(extendsize);
        if (block == NULL) // extend_heap returns an error
        {
            return bp;
        }

    }

    place(block, asize);
    bp = header_to_payload(block);

    dbg_ensures(mm_checkheap(__LINE__));
    return bp;
} 

/*
 * <what does free do?>
 */
 //Takes a pointer to a payload and frees the block to which it is attached
 //Then coalesces block
void free(void *bp)
{
    // printf(" ** Free Started** \n");
    if (bp == NULL)
    {
        return;
    }

    block_t *block = payload_to_header(bp);
    size_t size = get_size(block);

    write_header(block, size, false);
    write_footer(block, size, false);

    coalesce(block);

}

/*
 * <what does realloc do?>
 */
 //Same as malloc is "ptr" is null
 //Otherwise reallocates memory with new size
void *realloc(void *ptr, size_t size)
{
    // printf(" ** REalloc Started** \n");
    block_t *block = payload_to_header(ptr);
    size_t copysize;
    void *newptr;

    // If size == 0, then free block and return NULL
    if (size == 0)
    {
        free(ptr);
        return NULL;
    }

    // If ptr is NULL, then equivalent to malloc
    if (ptr == NULL)
    {
        return malloc(size);
    }

    // Otherwise, proceed with reallocation
    newptr = malloc(size);
    // If malloc fails, the original block is left untouched
    if (newptr == NULL)
    {
        return NULL;
    }

    // Copy the old data
    copysize = get_payload_size(block); // gets size of old payload
    if(size < copysize)
    {
        copysize = size;
    }
    memcpy(newptr, ptr, copysize);

    // Free the old block
    free(ptr);

    return newptr;
}

/*
 * <what does calloc do?>
 */
 //Same as malloc excepts it initializes all bits of the payload to zero
void *calloc(size_t elements, size_t size)
{   
    // printf(" ** Calloc Started** \n");
    void *bp;
    size_t asize = elements * size;

    if (asize/elements != size)
    {    
        // Multiplication overflowed
        return NULL;
    }
    
    bp = malloc(asize);
    if (bp == NULL)
    {
        return NULL;
    }
    // Initialize all bits to 0
    memset(bp, 0, asize);

    return bp;
}

/******** The remaining content below are helper and debug routines ********/

/*
 * <what does extend_heap do?>
 */
 //Extends heap by given size (rounded up to the nearest double size)
static block_t *extend_heap(size_t size) 
{
    // printf(" ** Extend Heap Started** \n");
    void *bp;

    // Allocate an even number of words to maintain alignment
    size = round_up(size, dsize);
    if ((bp = mem_sbrk(size)) == (void *)-1)
    {
        return NULL;
    }
    
    // Initialize free block header/footer 
    block_t *block = payload_to_header(bp);
    write_header(block, size, false);
    write_footer(block, size, false);
    // Create new epilogue header
    block_t *block_next = find_next(block);
    write_header(block_next, 0, true);

    // Coalesce in case the previous block was free
    return coalesce(block);
}

/*
 * <what does coalesce do?>
 */
 //Combines the freed parameter block with any surrounding blocks that are also free
static block_t *coalesce(block_t * block) 
{   
    // printf(" ** Coalesce Started** \n");
    block_t *next_block = find_next(block);
    block_t *prev_block = find_prev(block);
    size_t prev_alloc = get_alloc(prev_block);
    if(prev_block==block){
        prev_alloc = true;
    }
    size_t next_alloc = get_alloc(next_block);
    if(prev_alloc&&next_alloc){
        prepend(block);
        return block;
    }
    // size_t the_size = get_size(block);
    else if(prev_alloc && !next_alloc){
        remove_free(next_block);
        size_t size_new = get_size(block) + get_size(next_block);
        write_header(block, size_new, false);
        write_footer(block, size_new, false);
    }
    else if(!prev_alloc && next_alloc){
        remove_free(prev_block);
        size_t size_new = get_size(block) + get_size(prev_block);
        write_header(prev_block, size_new, false);
        write_footer(prev_block, size_new, false);  

        block = prev_block;
    }
    else if(!prev_alloc && !next_alloc){
        remove_free(next_block);
        remove_free(prev_block);
        size_t size_new = get_size(next_block) + get_size(prev_block) + get_size(block);
        write_header(prev_block, size_new, false);
        write_footer(prev_block, size_new, false);

        block = prev_block;
    }
    // fill me in
    prepend(block);
    return block;
}

/*
 * <what does place do?>
 */
 //sets the parameter block as allocated in the heap and removes it from the free list
 //If size of the block minus the allocated size is larger than the min_block_size
 //Split the parameter block into the allocated block and a free block
static void place(block_t *block, size_t asize)
{
    // printf(" ** Place Started** \n");
    size_t csize = get_size(block);
    remove_free(block);
    if ((csize - asize) >= min_block_size)
    {
        block_t *block_next;
        write_header(block, asize, true);
        write_footer(block, asize, true);

        // remove_free(block);

        block_next = find_next(block);

        write_header(block_next, csize-asize, false);
        write_footer(block_next, csize-asize, false);

        prepend(block_next);
    }

    else
    { 
        write_header(block, csize, true);
        write_footer(block, csize, true);
        // remove_free(block);
    }
    
}

/*
 * <what does find_fit do?>
 */
 //Searches the list of free blocks for the first non-null block with a size greater than the parameter
 //Returns null if no such block is found
static block_t *find_fit(size_t asize)
{
    block_t *block;
    for (block = free_listp; block!=NULL && get_size(block) > 0;block = block->next)
    {
        if (asize <= get_size(block))
        {
            return block;
        }
    }
    return NULL; // no fit found
}

/* 
 * <what does your heap checker do?>
 * Please keep modularity in mind when you're writing the heap checker!
 */
 //See inner function comments and comments in helper function -->
 //-->for what my heap checker does 
bool mm_checkheap(int line)  
{ 
    block_t *block;
    int heap_free = 0, list_free = 0;
    block = heap_start;                  

    //size and allocation of first block is valid 
    if ((get_size(block) != min_block_size) || (get_alloc(block) != 1)) {
        printf("--the first block is invalid--");
        assert(0);
        return false;
    }
    block = find_next(block);

    //Loop through all blocks in the heap 
    while (!((get_size(block) == 0) && (get_alloc(block) == 1))) {

        //run helper checking function on each block
        if(!checkblock(block)){
            return false;
        }

        //ensures coalesce works
        if (!get_alloc(block) && !get_alloc(find_next(block))) {
            printf("--Coalescing did not occur--");
            assert(0);
            return false;
        }

        //increment free block counter for heap
        if(!get_alloc(block))
            heap_free++;

        block = find_next(block);
    }

    //Now we check the free list
    block = free_listp;     

    //Nothing to check
    if (block == NULL) {
        return true;
    }

    //Iterate through free list
    while (block != NULL) {

        //Check list is doubly linked
        if (block->next->prev!=block->prev->next) {
            printf("List is not doubly linked");
            assert(0);
            return false;
        }
        
        // Check all blocks are free
        if (get_alloc(block)) {
            printf("Block in free list is not free");
            assert(0);
            return false;
        }

        //Increment free block counter in free list
        list_free++; 
        block = block->next;
    }

    if (heap_free != list_free) {
        printf("Discrepancy in free block count");
        assert(0);
        return false;
    }


    // (void)line; // delete this line; it's a placeholder so that the compiler
    //             // will not warn you about an unused variable.
    return true;
}

/*
 * max: returns x if x > y, and y otherwise.
 */
static size_t max(size_t x, size_t y)
{
    return (x > y) ? x : y;
}

/*
 * round_up: Rounds size up to next multiple of n
 */
static size_t round_up(size_t size, size_t n)
{
    return (n * ((size + (n-1)) / n));
}

/*
 * pack: returns a header reflecting a specified size and its alloc status.
 *       If the block is allocated, the lowest bit is set to 1, and 0 otherwise.
 */
static word_t pack(size_t size, bool alloc)
{
    return alloc ? (size | alloc_mask) : size;
}


/*
 * extract_size: returns the size of a given header value based on the header
 *               specification above.
 */
static size_t extract_size(word_t word)
{
    return (word & size_mask);
}

/*
 * get_size: returns the size of a given block by clearing the lowest 4 bits
 *           (as the heap is 16-byte aligned).
 */
static size_t get_size(block_t *block)
{
    return extract_size(block->header);
}

/*
 * get_payload_size: returns the payload size of a given block, equal to
 *                   the entire block size minus the header and footer sizes.
 */
static word_t get_payload_size(block_t *block)
{
    size_t asize = get_size(block);
    return asize - dsize;
}

/*
 * extract_alloc: returns the allocation status of a given header value based
 *                on the header specification above.
 */
static bool extract_alloc(word_t word)
{
    return (bool)(word & alloc_mask);
}

/*
 * get_alloc: returns true when the block is allocated based on the
 *            block header's lowest bit, and false otherwise.
 */
static bool get_alloc(block_t *block)
{
    return extract_alloc(block->header);
}

/*
 * write_header: given a block and its size and allocation status,
 *               writes an appropriate value to the block header.
 */
static void write_header(block_t *block, size_t size, bool alloc)
{
    block->header = pack(size, alloc);
}


/*
 * write_footer: given a block and its size and allocation status,
 *               writes an appropriate value to the block footer by first
 *               computing the position of the footer.
 */
static void write_footer(block_t *block, size_t size, bool alloc)
{
    word_t *footerp = (word_t *)((block->payload) + get_size(block) - dsize);
    *footerp = pack(size, alloc);
}


/*
 * find_next: returns the next consecutive block on the heap by adding the
 *            size of the block.
 */
static block_t *find_next(block_t *block)
{
    dbg_requires(block != NULL);
    block_t *block_next = (block_t *)(((char *)block) + get_size(block));

    dbg_ensures(block_next != NULL);
    return block_next;
}

/*
 * find_prev_footer: returns the footer of the previous block.
 */
static word_t *find_prev_footer(block_t *block)
{
    // Compute previous footer position as one word before the header
    return (&(block->header)) - 1;
}

/*
 * find_prev: returns the previous block position by checking the previous
 *            block's footer and calculating the start of the previous block
 *            based on its size.
 */
static block_t *find_prev(block_t *block)
{
    word_t *footerp = find_prev_footer(block);
    size_t size = extract_size(*footerp);
    return (block_t *)((char *)block - size);
}

/*
 * payload_to_header: given a payload pointer, returns a pointer to the
 *                    corresponding block.
 */
static block_t *payload_to_header(void *bp)
{
    return (block_t *)(((char *)bp) - offsetof(block_t, payload));
}

/*
 * header_to_payload: given a block pointer, returns a pointer to the
 *                    corresponding payload.
 */
static void *header_to_payload(block_t *block)
{
    return (void *)(block->payload);
}

  


//removes free block from free block list
static void remove_free(block_t *block){
    if(block==NULL){
        return;
    }
    //if only block in list
    if((block->prev==NULL)&&(block->next==NULL)){
        free_listp=NULL;
    }
    //if first block in list
    else if(block->prev==NULL){
        free_listp = block->next;
        block->next->prev=NULL;
    }
    //if last block in list
    else if(block->next==NULL){
        block->prev->next = NULL;
    }
    //generic case
    else{
        block->prev->next = block->next;
        block->next->prev = block->prev;
    }
    block->next = NULL;
    block->prev = NULL;
}

//adds parameter block to the start of the list of free blocks
static void prepend(block_t *block){
    if(block==NULL){
        return;
    }

    block->prev = NULL;
    block->next = free_listp;
    if(free_listp!=NULL){
        free_listp->prev = block;
    }
    free_listp=block;

}

//prints heap (not called)
static void print_heap(void){
    block_t * b;
    for(b = heap_start; get_size(b) != 0;b = find_next(b)){
        printf("%p:\tsize: %lu\talloc: %d",b, get_size(b), get_alloc(b));
        if (get_alloc(b)){
            printf("\n");
        }
        else{
             printf("\tprev: %p\tnext: %p\n", b->prev, b->next);
        }

    }
}

//prints free list (not called)
static void print_free(void){
    block_t * b;
    for(b = free_listp; get_size(b) != 0&& b!=NULL;b = b->next){
        printf("%p:\tsize: %lu\talloc: %d",b, get_size(b), get_alloc(b));
        if (get_alloc(b)){
            printf("\n");
        }
        else{
             printf("\tprev: %p\tnext: %p\n", b->prev, b->next);
        }

    }
}

static bool checkblock(block_t *block)  {

    //Check bounds of block
    if (find_prev(block)==block) { 
        printf("Bounds incorrect");
        assert(0);
        return false;
    }
    //Min-size check
    if (get_alloc(block)) {
        if (get_size(block) < min_block_size) {
            printf("Minimum size requirement not met");
            assert(0);
            return false;
        }
    }
    //Check alignment of header and footer
    if (get_size(block) % dsize)  {
        printf("alignment not correct");
        assert(0);
        return false;
    }
    return true;
}
