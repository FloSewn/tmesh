#ifndef darray_algos_h
#define darray_algos_h

#include <lcthw/darray.h>

typedef int (*DArray_compare) (const void *a, const void *b);

/*************************************************************
* QSort-Algorithm for dynamic arrays
*************************************************************/
int DArray_qsort(DArray *array, DArray_compare cmp);

/*************************************************************
* Heapsort-Algorithm for dynamic arrays
*************************************************************/
int DArray_heapsort(DArray *array, DArray_compare cmp);

/*************************************************************
* Mergesort-Algorithm for dynamic arrays
*************************************************************/
int DArray_mergesort(DArray *array, DArray_compare cmp);


#endif /*darray_algos_h*/
