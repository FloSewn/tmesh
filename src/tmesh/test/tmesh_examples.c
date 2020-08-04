#include "tmesh/tmNode.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmTri.h"
#include "tmesh/tmBdry.h"
#include "tmesh/tmMesh.h"
#include "tmesh/tmQtree.h"
#include "tmesh/tmFront.h"
#include "tmesh/tmList.h"

#include "tmesh/minunit.h"
#include "tmesh/dbg.h"

#include "tmesh_tests.h"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

/************************************************************
* Example 1)
*   Simple mesh with constant size
************************************************************/ 
char *test_example_1()
{
  /*--------------------------------------------------------
  | 1) Define the mesh structure.
  |
  |    xyMin, xyMax: The bounding box of the entire mesh
  |    nElemQtree  : number of elements in a QuadTree, 
  |                  until it will be refined
  |                  Has only influence on the performance.
  |    globSize    : A number > 0 defining the global 
  |                  element size of the mesh
  --------------------------------------------------------*/
  int      nElemQtree  = 50;
  tmDouble globSize    = 1.0;
  tmDouble xyMin[2]    = {-10.0,-10.0 };
  tmDouble xyMax[2]    = { 20.0, 20.0 };

  tmMesh *mesh = tmMesh_create(xyMin, xyMax, nElemQtree, 
                               globSize, NULL);

  /*--------------------------------------------------------
  | 2) Define the mesh's exterior boundary.
  |    The "FALSE" flag simply defines, that this boundary 
  |    is not an interior boundary.
  |    The 0 is an identifier for this boundary, which is 
  |    given as output later, as the mesh is printed out.
  --------------------------------------------------------*/
  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);

  /*--------------------------------------------------------
  | 3) Define the exterior boundary edge nodes.
  |    Each boundary edge segment consists of two directed 
  |    nodes (n1,n2), which must be defined first.
  |    IMPORTANT: Exterior boundary edges must be defined
  |               in a counter-clockwise manner.
  --------------------------------------------------------*/
  tmDouble xy0[2] = { -1.0, -1.0 };
  tmDouble xy1[2] = { 16.0,  7.0 };
  tmDouble xy2[2] = { -1.0, 10.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);

  /*--------------------------------------------------------
  | 4) Define the exterior boundary edge segments.
  |    The function takes the following parameters:

  |     > tmBdry_edgeCreate(bdry_ext, n0, n1, marker, f)
  |
  |    marker       : This is a further indicator which 
  |                   can be applied to each edge segment
  |                   Here, all segments have the same 
  |                   indicator, but it could also be 
  |                   chosen in an arbitrary manner, 
  |                   depending on the application the 
  |                   mesh will be used for.
  |                   This value is not related to the 
  |                   boundary identifier, which was also
  |                   chosen to be 1 above.
  |    refinement f : This factor > 1.0 defines the
  |                   refinement of the mesh elements near
  |                   the first edge node n1. Increasing
  |                   the value will lead to a larger 
  |                   refinement of elements near edges
  |                   with sharp angles.
  --------------------------------------------------------*/
  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0, 1.2);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0, 1.2);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n0, 0, 1.2);

  /*--------------------------------------------------------
  | 2) Define the mesh's interior boundary.
  |    The "TRUE" flag defines, that this boundary 
  |    is an interior boundary.
  |    The 1 is an identifier for this boundary, which is 
  |    given as output later, as the mesh is printed out.
  --------------------------------------------------------*/
  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE, 1);

  /*--------------------------------------------------------
  | 3) Define the interior boundary edge nodes.
  --------------------------------------------------------*/
  tmDouble xy3[2] = {  1.0,  4.0 };
  tmDouble xy4[2] = {  6.0,  8.0 };
  tmDouble xy5[2] = {  6.0,  4.0 };

  tmNode *n3 = tmNode_create(mesh, xy3);
  tmNode *n4 = tmNode_create(mesh, xy4);
  tmNode *n5 = tmNode_create(mesh, xy5);

  /*--------------------------------------------------------
  | 4) Define the interior boundary edge segments.
  |    IMPORTANT: Interior boundary edges must be defined
  |               in a clockwise manner.
  --------------------------------------------------------*/
  tmEdge *e3 = tmBdry_edgeCreate(bdry_int, n3, n4, 1, 1.6);
  tmEdge *e4 = tmBdry_edgeCreate(bdry_int, n4, n5, 1, 1.6);
  tmEdge *e5 = tmBdry_edgeCreate(bdry_int, n5, n3, 1, 1.6);

  /*--------------------------------------------------------
  | Create mesh
  --------------------------------------------------------*/
  clock_t tic_1 = clock();
  tmMesh_ADFMeshing(mesh);
  clock_t tic_2 = clock();

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);

  tmPrint("----------------------------------------------\n");
  tmPrint("Example 1 performance test                \n");
  tmPrint("----------------------------------------------\n");
  tmPrint("Number of elements: %d\n", mesh->no_tris);
  tmPrint("Meshing time      : %e sec\n", 
      (double) (tic_2 - tic_1) / CLOCKS_PER_SEC );
  tmPrint("----------------------------------------------\n");

  tmMesh_destroy(mesh);

  return NULL;
  
} /* test_example_1() */

/************************************************************
* Example 2)
*   Simple mesh with user defined size function
*
* First, we need to define a size function:
************************************************************/ 
static inline tmDouble sizeFunExample( tmDouble xy[2] )
{
  tmDouble sizeFac = 1.5;
  tmDouble dx = 8.0 - xy[0];
  tmDouble dy = 6.0 - xy[1];
  return sizeFac * (0.46 - 0.41 * exp(-0.05*(dx*dx+dy*dy)));
}
char *test_example_2()
{
  /*--------------------------------------------------------
  | 1) Define the mesh structure.
  |    The size function is now passed to the mesh as
  |    argument on its creation.
  |    The rest is done as before.   
  --------------------------------------------------------*/
  int      nElemQtree  = 50;
  tmDouble globSize    = 1.0;
  tmDouble xyMin[2]    = {-10.0,-10.0 };
  tmDouble xyMax[2]    = { 20.0, 20.0 };

  tmMesh *mesh = tmMesh_create(xyMin, xyMax, nElemQtree, 
                               globSize, sizeFunExample);

  /*--------------------------------------------------------
  | 2) Define the mesh's exterior boundary.
  --------------------------------------------------------*/
  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);

  /*--------------------------------------------------------
  | 3) Define the exterior boundary edge nodes.
  --------------------------------------------------------*/
  tmDouble xy0[2] = { -1.0, -1.0 };
  tmDouble xy1[2] = { 16.0,  7.0 };
  tmDouble xy2[2] = { -1.0, 10.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);

  /*--------------------------------------------------------
  | 4) Define the exterior boundary edge segments.
  --------------------------------------------------------*/
  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0, 1.2);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0, 1.2);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n0, 0, 1.2);

  /*--------------------------------------------------------
  | 2) Define the mesh's interior boundary.
  --------------------------------------------------------*/
  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE, 1);

  /*--------------------------------------------------------
  | 3) Define the interior boundary edge nodes.
  --------------------------------------------------------*/
  tmDouble xy3[2] = {  1.0,  4.0 };
  tmDouble xy4[2] = {  6.0,  8.0 };
  tmDouble xy5[2] = {  6.0,  4.0 };

  tmNode *n3 = tmNode_create(mesh, xy3);
  tmNode *n4 = tmNode_create(mesh, xy4);
  tmNode *n5 = tmNode_create(mesh, xy5);

  /*--------------------------------------------------------
  | 4) Define the interior boundary edge segments.
  --------------------------------------------------------*/
  tmEdge *e3 = tmBdry_edgeCreate(bdry_int, n3, n4, 1, 1.6);
  tmEdge *e4 = tmBdry_edgeCreate(bdry_int, n4, n5, 1, 1.6);
  tmEdge *e5 = tmBdry_edgeCreate(bdry_int, n5, n3, 1, 1.6);

  /*--------------------------------------------------------
  | Create mesh
  --------------------------------------------------------*/
  clock_t tic_1 = clock();
  tmMesh_ADFMeshing(mesh);
  clock_t tic_2 = clock();

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);

  tmPrint("----------------------------------------------\n");
  tmPrint("Example 2 performance test                \n");
  tmPrint("----------------------------------------------\n");
  tmPrint("Number of elements: %d\n", mesh->no_tris);
  tmPrint("Meshing time      : %e sec\n", 
      (double) (tic_2 - tic_1) / CLOCKS_PER_SEC );
  tmPrint("----------------------------------------------\n");

  tmMesh_destroy(mesh);

  return NULL;



} /* test_example_2() */
