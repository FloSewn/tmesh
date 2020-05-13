#include <assert.h>
#include "lcthw/minunit.h"
#include "lcthw/dbg.h"
#include "trimesh_tests.h"

/************************************************************
* Run all unit test functions
************************************************************/
char *all_tests()
{
  mu_suite_start();

  /**********************************************************
  * Unit tests for trimesh library
  **********************************************************/
  mu_run_test(test_mesh_create_destroy);
  mu_run_test(test_tmQtree);

  return NULL;
}

  
/************************************************************
* Main function to run unit tests
************************************************************/
int main(int argc, char *argv[])
{
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
