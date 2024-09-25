/*
 * uarray2.c 
 *
 * Assignment: III
 * Authors: hdoan02, msimps03
 * Date: 02/12/2022
 *
 * 2D array is implemented as a UArray_T which contains UArray_T's. Each inner 
 * UArray_T holds the actual data. The implementation uses col as the primary 
 * dimension to access the inner UArray_T. Row is the secondary dimension 
 * to access the data within each inner UArray_T.
 */
#include <uarray.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <assert.h>
#include "uarray2.h"

#define T UArray2_T

#define PRIDIM width
#define PRIDIM_ACCESSOR col
#define SECDIM height
#define SECDIM_ACCESSOR row

struct T {
    UArray_T array;
    int width;
    int height;
    int size;
};

/*
 * UArray2_new
 * 
 * Initialize a UArray2_T struct which holds the 2D array, width, height, and
 * size of each element. 
 * 
 * @param int width    - Width of the 2D array
 * @param int height   - Height of the 2D array
 * @param int size     - Size for each element in the 2D array
 * @return uarray2     - Pointer to struct containing the information about
 *                       the 2D array
 *
 * @expect             - If any of the input arguments is not positive, an 
 *                       error is raised
 */
T UArray2_new(int width, int height, int size)
{
    assert(width > 0 && height > 0 && size > 0);

    T uarray2;
    NEW(uarray2);
    
    UArray_T outer = UArray_new(PRIDIM, sizeof(UArray_T));
    for (int i = 0; i < PRIDIM; i++) {
        UArray_T *ptr = UArray_at(outer, i);
        *ptr = UArray_new(SECDIM, size);
    }

    uarray2->width = width;
    uarray2->height = height;
    uarray2->size = size;
    uarray2->array = outer;
    return uarray2;
}

/*
 * UArray2_free
 *
 * Deallocates and clears *uarray2. It is a checked runtime error
 * for uarray2 or *uarray2 to be null. 
 * 
 * @param T *uarray2 - Address to a pointer of type UArray2_T
 * @return None
 *
 * @expect           - An error is raised if uarray2 or *uarray2 is null
 */
void UArray2_free(T *uarray2)
{
    assert(uarray2 != NULL && *uarray2 != NULL);

    T duarray2 = *uarray2;
    UArray_T outer = duarray2->array;
    for (int i = 0; i < duarray2->PRIDIM; i++) {
        UArray_T *inner = UArray_at(outer, i);
        UArray_free(inner);
    }
    
    UArray_free(&outer);
    FREE(duarray2);
}

/*
 * UArray2_height
 * 
 * Gets the height of the 2D array.
 *
 * @param T uarray2  - Pointer of type T 
 * @return int       - Height of the 2D array
 *
 * @expect           - uarray2 to be non-null
 */
int UArray2_height(T uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->height;
}

/*
 * UArray2_widtht
 * 
 * Gets the width of the 2D array.
 *
 * @param T uarray2  - Pointer of type T 
 * @return int       - Width of the 2D array
 *
 * @expect           - uarray2 to be non-null
 */
int UArray2_width(T uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->width;
}

/*
 * UArray2_size
 * 
 * Gets the size of each element in the 2D array.
 *
 * @param T uarray2  - Pointer of type T 
 * @return int       - Size of each element in the 2D array
 *
 * @expect           - uarray2 to be non-null
 */
int UArray2_size(T uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->size;
}

/*
 * UArray2_at
 *
 * Gets the element at the specified coordinante in the 2D array. The column
 * is used to access the inner UArray_T. The row is used to access the element
 * within the inner UArray_T.
 *
 * @param T uarray2 - The 2D array to be selected from
 * @param int col   - The column (from the left) to select from
 * @param int row   - The row (from the top) to select from
 * @return void *   - The pointer to the specified element in the 2D array 
 *
 * @expect          - col is in range [0, Uarray2_width(uarray2))
 *                  - row is in range [0, UArrary2_height(uarray2))
 *                  - If client assigns element to pointer, the size must match 
 *                    UArray2_size(uarray2). Accessing out of bound coordinate
 *                    raises an error
 */
void *UArray2_at(T uarray2, int col, int row)
{
    assert(uarray2 != NULL);
    assert(col >= 0 && col < uarray2->width);
    if (!(row >= 0 && row < uarray2->height)) {
        fprintf(stderr, "Ugh oh, issue at col %d, row %d\n\n\n", col, row);
    }
    assert(row >= 0 && row < uarray2->height);

    UArray_T outer = uarray2->array;
    UArray_T inner = *((UArray_T *) UArray_at(outer, PRIDIM_ACCESSOR));
    return UArray_at(inner, SECDIM_ACCESSOR);
}

/*
 * UArray2_map_row_major
 *
 * Applies a function over each row and column in the 2D array. Column indices
 * vary more rapidly than row indices.
 *
 * @param T uarray2 - The 2D array to map
 * @param apply     - Client-defined function to be applied to each element in
 *                    the 2D array in the specified order. It takes in the
 *                    2D array, the current row, column, the pointer to the
 *                    current element, and the closure pointer
 * @param *cl       - Closure pointer to store across each apply operation 
 * @return None
 *
 * @expect          - uarray2 is non-null
 */
void UArray2_map_row_major(T uarray2,
                           void apply(int col, int row, T uarray2,
                                      void *el, void *cl),
                           void *cl)
{
    assert(uarray2 != NULL);
    for (int i = 0; i < uarray2->height; i++) { /* row */
        for (int j = 0; j < uarray2->width; j++) { /* col */
            apply(j, i, uarray2, UArray2_at(uarray2, j, i), cl);
        }
    }
}

/*
 * UArray2_map_col_major
 *
 * Applies a function over each row and column in the 2D array. Row indices
 * vary more rapidly than row indices.
 *
 * @param T uarray2 - The 2D array to map
 * @param apply     - Client-defined function to be applied to each element in
 *                    the 2D array in the specified order. It takes in the
 *                    2D array, the current row, column, the pointer to the
 *                    current element, and the closure pointer
 * @param *cl       - Closure pointer to store across each apply operation 
 * @return None
 *
 * @expect          - uarray2 is non-null
 */
void UArray2_map_col_major(T uarray2,
                           void apply(int col, int row, T uarray2,
                                      void *el, void *cl),
                           void *cl)
{
    assert(uarray2 != NULL);
    for (int i = 0; i < uarray2->width; i++) { /* col */
        for (int j = 0; j < uarray2->height; j++) {  /* row */
            apply(i, j, uarray2, UArray2_at(uarray2, i, j), cl);
        }
    }
}