#include <string.h>

#include "a2plain.h"
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

static A2Methods_UArray2 new(int width, int height, int size)
{
    return UArray2_new(width, height, size);
}

static A2Methods_UArray2 new_with_blocksize(int width, int height, int size, 
                                            int blocksize)
{
    (void) blocksize;
    return UArray2_new(width, height, size);
}

/* free */
static void a2free(A2Methods_UArray2* uarray2p)
{
	UArray2_free((UArray2_T *) uarray2p);
}

/* width */
static int width(A2Methods_UArray2 uarray2)
{
    return UArray2_width(uarray2);
}

/* height */
static int height(A2Methods_UArray2 uarray2)
{
    return UArray2_height(uarray2);
}

/* size */
static int size(A2Methods_UArray2 uarray2)
{
    return UArray2_size(uarray2);
}

/* blocksize */
static int blocksize(A2Methods_UArray2 uarray2)
{
    (void) uarray2;
    return 1;
}

/* at */
static A2Methods_Object *at(A2Methods_UArray2 uarray2, int i, int j)
{
    return UArray2_at(uarray2, i, j);
}

// typedef void UArray2_applyfun(int i, int j, UArray2_T array2,
//                               void *el, void *cl);

static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
  A2Methods_smallapplyfun *apply; 
  void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
  struct small_closure *cl = vcl;
  (void)i;
  (void)j;
  (void)uarray2;
  cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_col_major(a2, apply_small, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
    new,
    new_with_blocksize,
    a2free,
    width,
    height,
    size,
    blocksize,
    at,
    map_row_major,
    map_col_major,
    NULL,                   /* map_block_major */
    map_col_major,	        /* map_default */
    small_map_row_major,
    small_map_col_major,
    NULL,                   /* map_small_block_major */
    small_map_col_major,	/* map_small_default */
};

// finally the payoff: here is the exported pointer to the struct
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
