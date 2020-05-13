#include <lcthw/darray_algos.h>
#include <stdlib.h>

/*************************************************************
* QSort-Algorithm for dynamic arrays
*************************************************************/
int DArray_qsort(DArray *array, DArray_compare cmp)
{
  qsort(array->contents, DArray_count(array), 
      sizeof(void *), cmp);
  return 0;
}

/*************************************************************
* Heapsort-Algorithm for dynamic arrays
*************************************************************/
int DArray_heapsort(DArray *array, DArray_compare cmp)
{
  //return heapsort(array->contents, DArray_count(array),
  //    sizeof(void *), cmp);
  return 1;
}


/*************************************************************
* Mergesort-Algorithm for dynamic arrays
*************************************************************/
int DArray_mergesort(DArray *array, DArray_compare cmp)
{
  //return mergesort(array->contents, DArray_count(array),
  //    sizeof(void *), cmp);
  return 1;
}


