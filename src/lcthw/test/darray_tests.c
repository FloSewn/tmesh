#include <lcthw/minunit.h>
#include <lcthw/darray.h>
#include "darray_tests.h"

/************************************************************
* Definition of unit test variables 
************************************************************/
static DArray *array = NULL;
static int *val1 = NULL;
static int *val2 = NULL;

/************************************************************
* Definition of unit test functions 
************************************************************/
char *test_darray_create()
{
  array = DArray_create(sizeof(int), 100);
  
  mu_assert(array != NULL, "DArray_create failed.");
  mu_assert(array->contents != NULL, 
      "contents are wrong in darray.");
  mu_assert(array->end == 0, 
      "DArray end isn't at the right spot.");
  mu_assert(array->element_size == sizeof(int),
      "DArray element_size is wrong.");
  mu_assert(array->max == 100, 
      "DArray max is wrong.");

  return NULL;
}

char *test_darray_destroy()
{
  DArray_destroy(array);

  return NULL;
}

char *test_darray_new()
{
  val1 = DArray_new(array);
  mu_assert(val1 != NULL, "failed to make a new element.");

  val2 = DArray_new(array);
  mu_assert(val2 != NULL, "failed to make a new element.");

  return NULL;
}

char *test_darray_set()
{
  DArray_set(array, 0, val1);
  DArray_set(array, 1, val2);

  return NULL;
}

char *test_darray_get()
{
  mu_assert(DArray_get(array, 0) == val1, 
      "Wrong first value.");
  mu_assert(DArray_get(array, 1) == val2, 
      "Wrong first value.");

  return NULL;
}

char *test_darray_remove()
{
  int *val_check =  DArray_remove(array, 0);
  mu_assert(val_check != NULL, "Should not get NULL.");
  mu_assert(*val_check == *val1, "Should get the first value.");
  mu_assert(DArray_get(array, 0) == NULL, "Should be gone.");
  DArray_free(val_check);

  val_check =  DArray_remove(array, 1);
  mu_assert(val_check != NULL, "Should not get NULL.");
  mu_assert(*val_check == *val2, "Should get the first value.");
  mu_assert(DArray_get(array, 1) == NULL, "Should be gone.");
  DArray_free(val_check);

  return NULL;

}

char *test_darray_expand_contract()
{
  int old_max = array->max;
  DArray_expand(array);
  mu_assert((unsigned int)array->max == old_max + array->expand_rate, 
      "Wrong size after expand.");

  DArray_contract(array);
  mu_assert((unsigned int)array->max == array->expand_rate + 1, 
      "Should stay at expand rate at least.");

  DArray_contract(array);
  mu_assert((unsigned int)array->max == array->expand_rate + 1, 
      "Should stay at expand rate at least.");
  
  return NULL;
}

char *test_darray_push_pop()
{
  int i = 0;
  for (i = 0; i < 1000; i++) {
    int *val = DArray_new(array);
    *val = i * 333;
    DArray_push(array, val);
  }

  mu_assert(array->max == 1201, "Wrong max size.");

  for (i = 999; i >= 0; i--) {
    int *val = DArray_pop(array);
    mu_assert(val != NULL, "Shouldn't get a NULL.");
    mu_assert(*val == i * 333, "Wrong value.");
    DArray_free(val);
  }

  return NULL;
}
