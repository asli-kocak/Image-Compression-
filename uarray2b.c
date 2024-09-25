/* 
 * UARRAY2B.C
 *
 * CS40 HW3: LOCALITY
 * REFERENCE IMPLEMENTATION
 * 
 * Solution provided to the UArray2b.c implementation
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <uarray.h>
#include <assert.h>
#include <math.h>
#include <mem.h>

#include <uarray2b.h>
#include <uarray2.h>

#define T UArray2b_T

struct T { /* represents a 2D array of cells each of size 'size' */
        int width, height;
        unsigned blocksize;
        unsigned size;
        UArray2_T blocks;
        /*
         * matrix of blocks, each blocksize * blocksize 
         *
         * matrix dimensions are width and height divided by blocksize,
         * rounded up
         *
         * a block is a Uarray_T of length blocksize * blocksize 
         * and size 'size'
         *
         * invariant relating cells in blocks to cells in the abstraction
         *  described in section on coordinate transformations below
         */
};

/*
 * UArray2b_new
 *
 * Initializes a UArray2b_T struct which holds the 2D array, width, heigh, and
 * size of each element.
 *
 * @param int width     - Width of the 2D array
 * @param int height    - Height of the 2D array
 * @param int size      - Size of each element in the 2D array
 * @param int blocksize - Side length of each square block
 * @return UArray2b_T   - Pointer to the struct containing the new 2D array
 *
 * @expect              - All parameters have value greater than 0
 */
T UArray2b_new(int width, int height, int size, int blocksize)
{
    assert(blocksize > 0);

    T array;
    NEW(array);
    array->width  = width;
    array->height = height;
    array->size   = size;
    array->blocksize = blocksize;
    array->blocks = UArray2_new((width  + blocksize - 1) / blocksize,
                                (height + blocksize - 1) / blocksize,
                                sizeof(UArray_T));

    int xblocks = UArray2_width (array->blocks); 
    int yblocks = UArray2_height(array->blocks);

    for (int i = 0; i < xblocks; i++) {
        for (int j = 0; j < yblocks; j++) {
            UArray_T *block = UArray2_at(array->blocks, i, j);
            *block = UArray_new(blocksize * blocksize, size);
                        
            if (0) {
                fprintf(stderr, "Allocated %p; put %p at %p\n",
                (void *)*block, 
                (void *)*(UArray_T*)UArray2_at(array->blocks, i, j),
                UArray2_at(array->blocks, i, j));
            }
        }
    }
    return array;
}

/*
 * UArray2b_new_64K_block
 *
 * Intializes a UArray2b_T struct with blocksize such that given the size, each
 * block takes up 64KB of memory.
 *
 * @param int width     - Width of the 2D array
 * @param int height    - Height of the 2D array
 * @param int size      - Size of each element in the 2D array
 * @return UArray2b_T   - Pointer to the struct containing the new 2D array
 *
 * @expect              - All parameters have value greater than 0
 */
T UArray2b_new_64K_block(int width, int height, int size)
{
    int blocksize = (int) floor( sqrt((double)(64 * 1024) / (double)size) );
        if (blocksize == 0) {
            blocksize = 1;
        }
        /*  assert as big as possible */
        assert((blocksize + 1) * (blocksize + 1) * size > 64 * 1024);
        if (size <= 64 * 1024) { /* but no bigger */
            assert(blocksize * blocksize * size <= 64 * 1024); 
        }
        return UArray2b_new(width, height, size, blocksize);
}

/*
 * UArray2_free
 *
 * Deallocates and clears *uarray2b. It is a checked runtime error
 * for uarray2b or *uarray2b to be null. 
 * 
 * @param T *uarray2b   - Address to a pointer of type UArray2_T
 * @return None
 *
 * @expect               - An error is raised if uarray2 or *uarray2 is null
 */
void UArray2b_free(T *uarray2b)
{
    int i;
    assert(uarray2b && *uarray2b);
    T array = *uarray2b;

    int xblocks = UArray2_width (array->blocks);
    int yblocks = UArray2_height(array->blocks);
    assert(UArray2_size(array->blocks) == sizeof(UArray_T));

    for (i = 0; i < xblocks; i++) {
        for (int j = 0; j < yblocks; j++) {
            UArray_T *p = UArray2_at(array->blocks, i, j);
            UArray_free(p);
        }
    }

    UArray2_free(&(*uarray2b)->blocks);
    FREE(*uarray2b); 
}

/*
 * UArray2b_height
 * 
 * Gets the height of the 2D array.
 *
 * @param T uarray2b - Pointer of type T 
 * @return int       - Height of the 2D array
 *
 * @expect           - uarray2b to be non-null
 */
int UArray2b_height(T uarray2b)
{
    assert(uarray2b != NULL);
    return uarray2b->height;
}

/*
 * UArray2b_width
 * 
 * Gets the width of the 2D array.
 *
 * @param T uarray2b - Pointer of type T 
 * @return int       - Width of the 2D array
 *
 * @expect           - uarray2b to be non-null
 */
int UArray2b_width(T uarray2b)
{
    assert(uarray2b != NULL);
    return uarray2b->width;
}

/*
 * UArray2b_size
 * 
 * Gets the size of each element in the 2D array.
 *
 * @param T uarray2b - Pointer of type T 
 * @return int       - Size of each element in the 2D array
 *
 * @expect           - uarray2b to be non-null
 */
int UArray2b_size(T uarray2b)
{
    assert(uarray2b != NULL);
    return uarray2b->size;
}

/*
 * UArray2b_blocksize
 * 
 * Gets the blocksize of the 2D array.
 *
 * @param T uarray2b - Pointer of type T 
 * @return int       - Blocksize of the 2D array
 *
 * @expect           - uarray2b to be non-null
 */
int UArray2b_blocksize(T uarray2b) {
    assert(uarray2b != NULL);
    return uarray2b->blocksize;
}

/*
 * UArray2b_at
 * 
 * Gets the element at index (col, row) of the 2D array.
 *
 * @param T uarray2b    - Pointer of type T 
 * @param int col       - Column index of the element to get
 * @param int row       - Row index of the element to get
 * @return void *       - Element pointer
 *
 * @expect              - uarray2b to be non-null
 *                      - col is in the range [0, width)
 *                      - row is in the range [0, height)
 */
void *UArray2b_at(T uarray2b, int i, int j)
{
    assert(i >= 0 && j >= 0);
    /* avoid unused cells */
    assert(i < uarray2b->width && j < uarray2b->height);

    int b  = uarray2b->blocksize;
    int bx = i / b;   /* block x coordinate */
    int by = j / b;   /* block y coordinate */

    UArray_T *blockp = UArray2_at(uarray2b->blocks, bx, by);
    return UArray_at(*blockp, (i % b) * b + j % b);
}

/*
 * UArray2b_map
 * 
 * Call apply function on given parameters 
 *
 * @param T uarray2b    - Pointer of type T 
 * @param void apply    - Function pointer to func applied to each element
 * @param void *cl      - Pointer to closure value accumulated in apply func
 * @return void         - Returns none: sets value of cl
 *
 * @expect              - uarray2b to be non-null
 *                      - apply to be non-null
 */
void UArray2b_map(T uarray2b,
                  void apply(int col, int row, T uarray2b, void *el, void*cl),
                  void *cl)
{
    assert(uarray2b != NULL);
    int       h      = uarray2b->height;
    int       w      = uarray2b->width;
    int       b      = uarray2b->blocksize;
    UArray2_T blocks = uarray2b->blocks;
    int       bw     = UArray2_width(blocks);
    int       bh     = UArray2_height(blocks);

    for (int bx = 0; bx < bw; bx++) {
        for (int by = 0; by < bh; by++) {
            UArray_T *blockp = UArray2_at(blocks, bx, by);
            UArray_T  block  = *blockp;
            int       len    = UArray_length(block);
            /* (i0, j0) correspond to upper left */
            /* corner of block (bx, by)          */
            int i0 = b * bx; 
            int j0 = b * by; 
            for (int cell = 0; cell < len; cell++) {
                int i = i0 + cell / b;
                int j = j0 + cell % b;
                /* measured overhead 0.5% to 1.5% */
                if (i < w && j < h) {
                    apply(i, j, uarray2b, 
                        UArray_at(block, cell), cl);
                }
            }
        }
    }
}

#undef T