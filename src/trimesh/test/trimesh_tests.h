#ifndef trimesh_tests_h
#define trimesh_tests_h

/*************************************************************
* Unit test function to handle creation and 
* destruction of a mesh structure
*************************************************************/
char *test_mesh_create_destroy();

/*************************************************************
* Unit test function to handle the tmQtree structure
*
* !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!!
* Maximum number of qtree-objects must be set to
*
*   TM_QTREE_MAX_OBJ == 3
*
* in order to get this test running
* !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!!
*
************************************************************/
char *test_tmQtree();

#endif
