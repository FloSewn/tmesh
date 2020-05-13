#ifndef _Darray_h
#define _Darray_h

#include <stdlib.h>
#include <assert.h>
#include <lcthw/dbg.h>


/*************************************************************
* DEFINES
*************************************************************/
#define DEFAULT_EXPAND_RATE 300

#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)
#define DArray_free(E) free((E))



/*************************************************************
* DArray Structure
*************************************************************/
typedef struct DArray {
  int end;
  int max;
  size_t element_size;
  size_t expand_rate;
  void **contents;
} DArray;

/*************************************************************
* Function to create a new DArray
*************************************************************/
DArray *DArray_create(size_t element_size, size_t initial_max);

/*************************************************************
* Function frees content pointer array and DArray struct memory
*************************************************************/
void DArray_destroy(DArray *array);

/*************************************************************
* Function frees memory for every content entry of DArray
*************************************************************/
void DArray_clear(DArray *array);

/*************************************************************
* Function to fully clear a DArray memory
*************************************************************/
void DArray_clear_destroy(DArray *array);

/*************************************************************
* Function to expand a DArray memory for its expand_rate
*************************************************************/
int DArray_expand(DArray *array);

/*************************************************************
* Function to contract a DArray memory for its expand_rate
*************************************************************/
int DArray_contract(DArray *array);

/*************************************************************
* Function pushes new value to the end of content of DArray
*************************************************************/
int DArray_push(DArray *array, void *el);

/*************************************************************
* Function pops value from the end of content of DArray
*************************************************************/
void *DArray_pop(DArray *array);




/*************************************************************
* Set an element <el> into the DArray at position <i>
*************************************************************/
static inline void DArray_set(DArray *array, int i, void *el)
{
  check(i < array->max, "darray attempt to set past max.");
  if (i > array->end)
    array->end = i;
  array->contents[i] = el;
error:
  return;
}

/*************************************************************
* Get an element <el> from the DArray at position <i>
*************************************************************/
static inline void *DArray_get(DArray *array, int i)
{
  check(i < array->max, "darray attempt to get past max");
  return array->contents[i];
error:
  return NULL;
}

static inline void *DArray_remove(DArray *array, int i)
{
  void *el = array->contents[i];
  array->contents[i] = NULL;

  return el;
}

/*************************************************************
* Create a new element that has the same type as all other
* elements which are stored in the DArray contents
*************************************************************/
static inline void *DArray_new(DArray *array)
{
  check(array->element_size > 0, 
      "Can't use DArray_new on 0 size darrays.");
  return calloc(1, array->element_size);

error:
  return NULL;
}

/*************************************************************
* Resizes the contents of DArray to new size
*************************************************************/
static inline int DArray_resize(DArray *array, size_t newsize)
{
  array->max = newsize;
  check(array->max > 0, "The newsize must be > 0.");

  void *contents = realloc(array->contents,
      array->max * sizeof(void *));
  // Check contents and assume realloc doesn't harm the 
  // original data

  check_mem(contents);

  array->contents = contents;

  return 0;
error:
  return -1;
}


#endif /* _Darray_h */
