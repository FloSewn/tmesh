#include <assert.h>
#include "tmesh/minunit.h"
#include "tmesh/dbg.h"
#include "tmesh_tests.h"
#include "tmList_tests.h"
#include "tmParam_tests.h"
#include "tmesh_cylinder.h"
#include "geom_tests.h"
#include "tmesh_examples.h"

/************************************************************
* Run all unit test functions
************************************************************/
char *all_tests()
{
  mu_suite_start();

  /**********************************************************
  * Unit tests for list structure
  **********************************************************/
  mu_run_test(test_tmList_create);
  mu_run_test(test_tmList_push_pop);
  mu_run_test(test_tmList_unshift);
  mu_run_test(test_tmList_remove);
  mu_run_test(test_tmList_shift);
  mu_run_test(test_tmList_join);
  mu_run_test(test_tmList_destroy);
  mu_run_test(test_tmList_split);

  /**********************************************************
  * Unit tests for parameter file reader
  **********************************************************/
  mu_run_test(test_tmParam_readfile);

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
  //mu_run_test(test_tmQtree_performance2);
  //mu_run_test(test_tmBdry_refine);
  //mu_run_test(test_tmFront_init);
  //mu_run_test(test_tmFront_advance); 
  //mu_run_test(test_tmFront_simpleMesh);
  //mu_run_test(test_tmFront_simpleMesh2);
  //mu_run_test(test_tmFront_innerOuterMesh);
  //mu_run_test(test_tmFront_tMesh);
    
  //mu_run_test(test_tmBdry_sizeFunction);
  
  //mu_run_test(test_mesh_cylinder);
  

  /**********************************************************
  * TMesh Examples
  **********************************************************/
  //mu_run_test(test_example_1);
  //mu_run_test(test_example_2);


  return NULL;
}

  
/************************************************************
* Main function to run unit tests
************************************************************/
int main(int argc, char *argv[])
{
  debug("----- RUNNING %s\n", argv[0]);
  
  char *result;
  result = all_tests();

  if (result != 0)
  {
    debug("FAILED: %s\n", result);
  }
  else
  {
    debug("\nALL TESTS PASSED!\n");
  }

  mu_print_tests_run();

  exit(result != 0);
}
