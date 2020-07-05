#include <assert.h>
#include "lcthw/minunit.h"
#include "lcthw/dbg.h"
#include "tmesh_tests.h"
#include "geom_tests.h"

/************************************************************
* Run all unit test functions
************************************************************/
char *all_tests()
{
  mu_suite_start();

  /**********************************************************
  * Unit tests for tmesh library
  **********************************************************/
  //mu_run_test(test_geometric_functions);
  //mu_run_test(test_tmTri_intersection);
  //mu_run_test(test_mesh_create_destroy);
  //mu_run_test(test_tmMesh_objInside);
  //mu_run_test(test_tmQtree);
  //mu_run_test(test_tmQtree_2);
  //mu_run_test(test_tmQtree_performance);
  //mu_run_test(test_tmBdry_refine);
  //mu_run_test(test_tmFront_init);
  //mu_run_test(test_tmFront_advance); 
  //mu_run_test(test_tmFront_simpleMesh);
  mu_run_test(test_tmFront_simpleMesh2);
  //mu_run_test(test_tmFront_innerOuterMesh);
  



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
