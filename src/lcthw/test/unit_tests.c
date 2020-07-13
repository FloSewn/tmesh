#include <assert.h>
#include "lcthw/minunit.h"
#include "lcthw/dbg.h"
#include "list_tests.h"
#include "list_algos_tests.h"
#include "darray_algos_tests.h"
#include "darray_tests.h"
#include "bstrlib_tests.h"
#include "txtio_tests.h"

/************************************************************
* Run all unit test functions
************************************************************/
char *all_tests()
{
  mu_suite_start();

  /**********************************************************
  * Unit tests for list structure
  **********************************************************/
  mu_run_test(test_list_create);
  mu_run_test(test_list_push_pop);
  mu_run_test(test_list_unshift);
  mu_run_test(test_list_remove);
  mu_run_test(test_list_shift);
  mu_run_test(test_list_join);
  mu_run_test(test_list_destroy);
  mu_run_test(test_list_split);

  /**********************************************************
  * Unit tests for list algorithms
  **********************************************************/
  mu_run_test(test_list_algos_bubble_sort);
  mu_run_test(test_list_algos_merge_sort);

  /**********************************************************
  * Unit tests for dynamic arrays
  **********************************************************/
  mu_run_test(test_darray_create);
  mu_run_test(test_darray_new);
  mu_run_test(test_darray_set);
  mu_run_test(test_darray_get);
  mu_run_test(test_darray_remove);
  
  mu_run_test(test_darray_expand_contract);
  mu_run_test(test_darray_push_pop);
  mu_run_test(test_darray_destroy);

  /**********************************************************
  * Unit tests for dynamic array algorithms
  **********************************************************/
  mu_run_test(test_darray_qsort);

  /**********************************************************
  * Unit tests for better strings library
  **********************************************************/
  mu_run_test(test_bstring_create_destroy);
  mu_run_test(test_bstring_concat);
  mu_run_test(test_bstring_compare);
  mu_run_test(test_bstring_list);

  /**********************************************************
  * Unit tests for file input / output library
  **********************************************************/
  mu_run_test(test_txtio_readfile);


  return NULL;
}

//RUN_TESTS(all_tests);
  
  
/************************************************************
* Main function to run unit tests
************************************************************/
int main(int argc, char *argv[])
{
  //debug("\n----- RUNNING %s\n", argv[0]);
  printf("----- RUNNING %s\n", argv[0]);
  
  char *result;
  result = all_tests();

  if (result != 0)
  {
    printf("FAILED: %s\n", result);
  }
  else
  {
    printf("ALL TESTS PASSED!\n");
  }

  mu_print_tests_run();

  exit(result != 0);
}
