#include "trimesh/tmNode.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmTri.h"
#include "trimesh/tmBdry.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmQtree.h"
#include "trimesh/tmFront.h"

#include "lcthw/list.h"
#include "lcthw/minunit.h"
#include "lcthw/dbg.h"

#include "trimesh_tests.h"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>


/*************************************************************
* Size function to test boundary refinement
*************************************************************/
static inline tmDouble size_fun_1( tmDouble xy[2] )
{
  return 0.05 + 0.5 * xy[0] + 0.5 * xy[1];
}
static inline tmDouble size_fun_2( tmDouble xy[2] )
{
  return 0.5;
}
static inline tmDouble size_fun_3( tmDouble xy[2] )
{
  tmDouble x0 = 64.0;
  tmDouble y0 = 55.0;
  tmDouble dx = x0 - xy[0];
  tmDouble dy = y0 - xy[1];
  tmDouble r2 = dx*dx + dy*dy;
  return 1.75 - 1.6 * exp(-0.003*r2);
}
static inline tmDouble size_fun_4( tmDouble xy[2] )
{
  return 3.;
}
static inline tmDouble size_fun_5( tmDouble xy[2] )
{
  tmDouble y0 = 10.0;
  tmDouble dy = y0 - xy[1];
  return 2.50 - 2.00 * exp(-0.02*dy*dy) + 0.02 * xy[0];
  //tmDouble val = 0.05
  //             + 0.001 * (xy[0]-50)*(xy[0]-50) 
  //             + 0.001 * (xy[1]-50)*(xy[1]-50);
  //return val;
}
static inline tmDouble size_fun_6( tmDouble xy[2] )
{
  return 2.;
}


/*************************************************************
* Unit test function to handle creation and 
* destruction of a trimesh structure
*************************************************************/
char *test_mesh_create_destroy()
{
  tmDouble xy_min[2] = {  -2.0, -2.0 };
  tmDouble xy_max[2] = {  22.0, 12.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 3, size_fun_1);

  /*--------------------------------------------------------
  | exterior nodes
  --------------------------------------------------------*/
  tmDouble xy_0[2] = {  0.0,  0.0 };
  tmDouble xy_1[2] = {  7.0,  0.0 };
  tmDouble xy_2[2] = { 13.0,  0.0 };
  tmDouble xy_3[2] = { 20.0,  0.0 };
  tmDouble xy_4[2] = { 20.0,  4.0 };
  tmDouble xy_5[2] = { 20.0, 10.0 };
  tmDouble xy_6[2] = { 14.0, 10.0 };
  tmDouble xy_7[2] = {  7.0, 10.0 };
  tmDouble xy_8[2] = {  0.0, 10.0 };
  tmDouble xy_9[2] = {  0.0,  5.0 };
  /*--------------------------------------------------------
  | interior nodes
  --------------------------------------------------------*/
  tmDouble xy_10[2] = {   9.0,  5.0 };
  tmDouble xy_11[2] = {  11.0,  7.0 };
  tmDouble xy_12[2] = {  13.0,  8.0 };
  tmDouble xy_13[2] = {  16.0,  8.0 };
  tmDouble xy_14[2] = {  17.0,  5.0 };
  tmDouble xy_15[2] = {  16.0,  2.0 };
  tmDouble xy_16[2] = {  13.0,  2.0 };
  tmDouble xy_17[2] = {  11.0,  3.0 };

  /*--------------------------------------------------------
  | Add nodes for exterior boundary
  --------------------------------------------------------*/
  tmNode *n_0 = tmNode_create(mesh, xy_0);
  tmNode *n_1 = tmNode_create(mesh, xy_1);
  tmNode *n_2 = tmNode_create(mesh, xy_2);
  tmNode *n_3 = tmNode_create(mesh, xy_3);
  tmNode *n_4 = tmNode_create(mesh, xy_4);
  tmNode *n_5 = tmNode_create(mesh, xy_5);
  tmNode *n_6 = tmNode_create(mesh, xy_6);
  tmNode *n_7 = tmNode_create(mesh, xy_7);
  tmNode *n_8 = tmNode_create(mesh, xy_8);
  tmNode *n_9 = tmNode_create(mesh, xy_9);

  mu_assert( List_first(mesh->nodes_stack) == n_0,
      "Failed to add node to mesh->nodes_stack.");
  mu_assert( List_last(mesh->nodes_stack) == n_9,
      "Failed to add node to mesh->nodes_stack."); 

  /*--------------------------------------------------------
  | Add nodes for interior boundary
  --------------------------------------------------------*/
  tmNode *n_10 = tmNode_create(mesh, xy_10);
  tmNode *n_11 = tmNode_create(mesh, xy_11);
  tmNode *n_12 = tmNode_create(mesh, xy_12);
  tmNode *n_13 = tmNode_create(mesh, xy_13);
  tmNode *n_14 = tmNode_create(mesh, xy_14);
  tmNode *n_15 = tmNode_create(mesh, xy_15);
  tmNode *n_16 = tmNode_create(mesh, xy_16);
  tmNode *n_17 = tmNode_create(mesh, xy_17);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);
  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE,  1);

  mu_assert( List_first(mesh->bdry_stack) == bdry_ext,
      "Failed to create boundary.");
  mu_assert( List_last(mesh->bdry_stack) == bdry_int,
      "Failed to create boundary.");

  /*--------------------------------------------------------
  | Create exterior boundary edges
  --------------------------------------------------------*/
  tmEdge *e_0 = tmBdry_edgeCreate(bdry_ext, n_0, n_1, 0);
  tmEdge *e_1 = tmBdry_edgeCreate(bdry_ext, n_1, n_2, 0);
  tmEdge *e_2 = tmBdry_edgeCreate(bdry_ext, n_2, n_3, 0);
  tmEdge *e_3 = tmBdry_edgeCreate(bdry_ext, n_3, n_4, 0);
  tmEdge *e_4 = tmBdry_edgeCreate(bdry_ext, n_4, n_5, 0);
  tmEdge *e_5 = tmBdry_edgeCreate(bdry_ext, n_5, n_6, 0);
  tmEdge *e_6 = tmBdry_edgeCreate(bdry_ext, n_6, n_7, 0);
  tmEdge *e_7 = tmBdry_edgeCreate(bdry_ext, n_7, n_8, 0);
  tmEdge *e_8 = tmBdry_edgeCreate(bdry_ext, n_8, n_9, 0);
  tmEdge *e_9 = tmBdry_edgeCreate(bdry_ext, n_9, n_0, 0);

  /*--------------------------------------------------------
  | Check if qtree works for boundary edges
  --------------------------------------------------------*/
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_SW, e_0, 0) == TRUE,
      "Edge e_0 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_SE, e_1, 0) == TRUE,
      "Edge e_1 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_SE, e_2, 0) == TRUE,
      "Edge e_2 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_SE, e_3, 0) == TRUE,
      "Edge e_3 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_NE, e_4, 0) == TRUE,
      "Edge e_4 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_NE, e_5, 0) == TRUE,
      "Edge e_5 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_NE, e_6, 0) == TRUE,
      "Edge e_6 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_NW, e_7, 0) == TRUE,
      "Edge e_7 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_NW, e_8, 0) == TRUE,
      "Edge e_8 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(bdry_ext->edges_qtree->child_SW, e_9, 0) == TRUE,
      "Edge e_9 has not been added to mesh qtree."); 

  /*--------------------------------------------------------
  | Check function for finding edges in qree
  --------------------------------------------------------*/
  tmDouble bbox_min[2] = { 13.0, -1.0 };
  tmDouble bbox_max[2] = { 22.0, 11.0 };

  List *obj_bbox = tmQtree_getObjBbox(bdry_ext->edges_qtree, 
                                      bbox_min, bbox_max);

  mu_assert(obj_bbox->count == 4,
      "tmQtree_getObjBbox() failed.");
  mu_assert(obj_bbox->first->value == e_4,
      "tmQtree_getObjBbox() failed.");
  mu_assert(obj_bbox->first->next->value == e_5,
      "tmQtree_getObjBbox() failed.");
  mu_assert(obj_bbox->last->prev->value == e_2,
      "tmQtree_getObjBbox() failed.");
  mu_assert(obj_bbox->last->value == e_3,
      "tmQtree_getObjBbox() failed.");

  if (obj_bbox != NULL)
    List_destroy(obj_bbox);

  /*--------------------------------------------------------
  | Create interior boundary edges
  --------------------------------------------------------*/
  tmEdge *e_10 = tmBdry_edgeCreate(bdry_int, n_10, n_11, 1);
  tmEdge *e_11 = tmBdry_edgeCreate(bdry_int, n_11, n_12, 1);
  tmEdge *e_12 = tmBdry_edgeCreate(bdry_int, n_12, n_13, 1);
  tmEdge *e_13 = tmBdry_edgeCreate(bdry_int, n_13, n_14, 1);
  tmEdge *e_14 = tmBdry_edgeCreate(bdry_int, n_14, n_15, 1);
  tmEdge *e_15 = tmBdry_edgeCreate(bdry_int, n_15, n_16, 1);
  tmEdge *e_16 = tmBdry_edgeCreate(bdry_int, n_16, n_17, 1);
  tmEdge *e_17 = tmBdry_edgeCreate(bdry_int, n_17, n_10, 1);
  
  /*--------------------------------------------------------
  | Check is left / left on functions
  --------------------------------------------------------*/
  mu_assert(tmEdge_isLeft( e_0, n_9, TM_NODE ) == TRUE,
      "tmEdge_isLeft() failed.");
  mu_assert(tmEdge_isLeftOn( e_0, n_1, TM_NODE ) == TRUE,
      "tmEdge_isLeft() failed."); 
  /*--------------------------------------------------------
  | Check boundary functions
  --------------------------------------------------------*/
  tmDouble xy_t_0[2] = {  6.0,  5.0 };
  tmDouble xy_t_1[2] = { 13.0,  5.0 };
  tmDouble xy_t_2[2] = { -2.0, -5.0 };
  tmNode *n_t_0 = tmNode_create(mesh, xy_t_0);
  tmNode *n_t_1 = tmNode_create(mesh, xy_t_1);
  tmNode *n_t_2 = tmNode_create(mesh, xy_t_2);

  mu_assert( tmBdry_isLeft(bdry_ext, n_t_0, TM_NODE) == TRUE,
      "tmBdry_isLeft() failed.");
  mu_assert( tmBdry_isLeftOn(bdry_ext, n_7, TM_NODE) == TRUE,
      "tmBdry_isLeftOn() failed.");
  mu_assert( tmBdry_isRight(bdry_int, n_t_0, TM_NODE) == FALSE,
      "tmBdry_isRight() failed.");

  mu_assert( tmBdry_isRight(bdry_int, n_t_1, TM_NODE) == TRUE,
      "tmBdry_isRight() failed.");
  mu_assert( tmBdry_isRightOn(bdry_int, n_11, TM_NODE) == TRUE,
      "tmBdry_isRightOn() failed.");
  mu_assert( tmBdry_isLeft(bdry_int, n_t_1, TM_NODE) == FALSE,
      "tmBdry_isLeft() failed.");


  mu_assert( tmMesh_objInside(mesh, n_t_0->xy) == TRUE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, n_t_1->xy) == FALSE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, n_t_2->xy) == FALSE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, n_2->xy) == TRUE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, n_12->xy) == FALSE,
      "tmMesh_objInside() failed.");


  tmMesh_destroy(mesh);

  return NULL;
} /* test_mesh_create_destroy() */


/************************************************************
* Unit test function to test if an object can be located
* within the domain
************************************************************/
char *test_tmMesh_objInside()
{

  tmDouble xy_min[2] = {  -5.0, -5.0 };
  tmDouble xy_max[2] = {  20.0, 20.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 10, size_fun_2);

  /*--------------------------------------------------------
  | exterior nodes
  --------------------------------------------------------*/
  tmDouble xy0[2] = {   0.0,   0.0 };
  tmDouble xy1[2] = {   8.0,   0.0 };
  tmDouble xy2[2] = {  11.0,  11.0 };
  tmDouble xy3[2] = {   7.0,   7.0 };
  tmDouble xy4[2] = {   3.0,   8.0 };
  tmDouble xy5[2] = {  -2.0,   5.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);
  tmNode *n3 = tmNode_create(mesh, xy3);
  tmNode *n4 = tmNode_create(mesh, xy4);
  tmNode *n5 = tmNode_create(mesh, xy5);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);

  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n3, 0);
  tmEdge *e3 = tmBdry_edgeCreate(bdry_ext, n3, n4, 0);
  tmEdge *e4 = tmBdry_edgeCreate(bdry_ext, n4, n5, 0);
  tmEdge *e5 = tmBdry_edgeCreate(bdry_ext, n5, n0, 0);

  /*--------------------------------------------------------
  | interior nodes
  --------------------------------------------------------*/
  tmDouble xy6[2]  = {   5.0,    2.0 };
  tmDouble xy7[2]  = {   1.0,    2.0 };
  tmDouble xy8[2]  = {   2.0,    3.0 };
  tmDouble xy9[2]  = {   1.0,    5.0 };
  tmDouble xy10[2] = {   4.0,    5.0 };
  tmDouble xy11[2] = {   7.0,    6.0 };
  tmDouble xy12[2] = {   5.0,    4.0 };

  tmNode *n6  = tmNode_create(mesh, xy6);
  tmNode *n7  = tmNode_create(mesh, xy7);
  tmNode *n8  = tmNode_create(mesh, xy8);
  tmNode *n9  = tmNode_create(mesh, xy9);
  tmNode *n10 = tmNode_create(mesh, xy10);
  tmNode *n11 = tmNode_create(mesh, xy11);
  tmNode *n12 = tmNode_create(mesh, xy12);

  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE, 1);

  tmEdge *e6  = tmBdry_edgeCreate(bdry_int, n6, n7, 1);
  tmEdge *e7  = tmBdry_edgeCreate(bdry_int, n7, n8, 1);
  tmEdge *e8  = tmBdry_edgeCreate(bdry_int, n8, n9, 1);
  tmEdge *e9  = tmBdry_edgeCreate(bdry_int, n9, n10, 1);
  tmEdge *e10 = tmBdry_edgeCreate(bdry_int, n10, n11, 1);
  tmEdge *e11 = tmBdry_edgeCreate(bdry_int, n11, n12, 1);
  tmEdge *e12 = tmBdry_edgeCreate(bdry_int, n12, n6, 1);

  /*--------------------------------------------------------
  | Test if point outside of the domain works
  --------------------------------------------------------*/
  tmDouble xyt0[2]  = {   7.0,    4.0 };
  tmDouble xyt1[2]  = {   4.0,    7.0 };
  tmDouble xyt2[2]  = {   0.0,    2.0 };
  tmDouble xyt3[2]  = {   3.0,    0.0 };
  tmDouble xyt4[2]  = {   3.0,    2.0 };
  tmDouble xyt5[2]  = {   2.0,    4.0 };

  tmNode *t0  = tmNode_create(mesh, xyt0);
  tmNode *t1  = tmNode_create(mesh, xyt1);
  tmNode *t2  = tmNode_create(mesh, xyt2);
  tmNode *t3  = tmNode_create(mesh, xyt3);
  tmNode *t4  = tmNode_create(mesh, xyt4);
  tmNode *t5  = tmNode_create(mesh, xyt5);

  tmBdry_refine(bdry_ext);
  tmBdry_refine(bdry_int);


  mu_assert( tmMesh_objInside(mesh, t0->xy) == TRUE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, t1->xy) == TRUE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, t2->xy) == TRUE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, t3->xy) == TRUE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, t4->xy) == FALSE,
      "tmMesh_objInside() failed.");
  mu_assert( tmMesh_objInside(mesh, t5->xy) == FALSE,
      "tmMesh_objInside() failed.");


  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  //tmMesh_printMesh(mesh);
  tmMesh_destroy(mesh);

  return NULL;



} /* test_tmMesh_objInside() */

/*************************************************************
* Unit test function to the boundary refinement
************************************************************/
char *test_tmBdry_refine()
{
  tmDouble xy_min[2] = {  -5.0, -5.0 };
  tmDouble xy_max[2] = {  15.0, 15.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 3, size_fun_3);

  /*--------------------------------------------------------
  | exterior nodes
  --------------------------------------------------------*/
  tmDouble xy0[2] = {  0.0,  0.0 };
  tmDouble xy1[2] = { 10.0,  0.0 };
  tmDouble xy2[2] = { 10.0,  5.0 };
  tmDouble xy3[2] = {  0.0,  5.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);
  tmNode *n3 = tmNode_create(mesh, xy3);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);
  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n3, 0);
  tmEdge *e3 = tmBdry_edgeCreate(bdry_ext, n3, n0, 0);

  tmBdry_calcArea(bdry_ext);
  tmPrint("bdry_ext area: %.5f", bdry_ext->area);
  mu_assert( EQ(bdry_ext->area, 50.0), 
      "Wrong area for exterior boundary.");

  /*--------------------------------------------------------
  | interior nodes
  --------------------------------------------------------*/
  tmDouble xy4[2] = {  2.0,  2.0 };
  tmDouble xy5[2] = {  3.0,  3.0 };
  tmDouble xy6[2] = {  5.0,  2.0 };

  tmNode *n4 = tmNode_create(mesh, xy4);
  tmNode *n5 = tmNode_create(mesh, xy5);
  tmNode *n6 = tmNode_create(mesh, xy6);

  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE, 1);
  tmEdge *e4 = tmBdry_edgeCreate(bdry_int, n4, n5, 1);
  tmEdge *e5 = tmBdry_edgeCreate(bdry_int, n5, n6, 1);
  tmEdge *e6 = tmBdry_edgeCreate(bdry_int, n6, n4, 1);

  tmBdry_calcArea(bdry_int);
  tmPrint("bdry_int area: %.5f", bdry_int->area);
  mu_assert( EQ(bdry_int->area, -1.5), 
      "Wrong area for interior boundary.");

  tmMesh_calcArea(mesh);
  tmPrint("mesh area: %.5f", mesh->areaBdry);
  mu_assert( EQ(mesh->areaBdry, 48.5),
      "Wrong area for mesh domain.");

  /*--------------------------------------------------------
  | Split edge e_0
  --------------------------------------------------------*
  e0 = tmBdry_splitEdge(bdry_ext, e0);

  tmNode *n4 = e0->n2;
  //mu_assert( n4->xy[0] == 5.5, 
  //    "tmBdry_splitEdge() failed.");
  //mu_assert( n4->xy[1] == 0.0, 
  //    "tmBdry_splitEdge() failed.");

  List *n4_e_in = tmNode_getBdryEdgeIn(n4);
  mu_assert( e0 == List_first(n4_e_in),
      "tmNode_getBdryEdgeIn() failed.");
  List_destroy(n4_e_in);

  List *n4_e_out = tmNode_getBdryEdgeOut(n4);
  tmEdge *e4 = List_first(n4_e_out);
  List_destroy(n4_e_out);

  mu_assert( e4->n1 == n4,
      "tmBdry_splitEdge() failed.");
  mu_assert( e4->n2 == n1,
      "tmBdry_splitEdge() failed.");
  */

  /*--------------------------------------------------------
  | Refine whole boundary according to size function
  --------------------------------------------------------*/
  tmBdry_refine(bdry_ext);

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);

  tmMesh_destroy(mesh);

  return NULL;

} /* test_tmBdry_refine() */


/************************************************************
* Unit test function to handle the tmQtree structure
*
* Maximum number of qtree-objects must be set to 2  
* in order to get this test running
************************************************************/
char *test_tmQtree_2()
{
  tmDouble xy_min[2] = {  0.0,  0.0 };
  tmDouble xy_max[2] = { 10.0, 10.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 2, size_fun_1);

  /*--------------------------------------------------------
  | Add new nodes 
  --------------------------------------------------------*/
  tmDouble xy_0[2]  = {  0.00,  0.00};
  tmDouble xy_1[2]  = { 10.00,  0.00};
  tmDouble xy_2[2]  = {  5.00,  0.00};
  tmDouble xy_3[2]  = {  2.50,  0.00};
  tmDouble xy_4[2]  = {  7.50,  0.00};
  tmDouble xy_5[2]  = {  1.25,  0.00};
  tmDouble xy_6[2]  = {  3.75,  0.00};
  tmDouble xy_7[2]  = {  6.25,  0.00};

  tmNode *n_0  = tmNode_create(mesh, xy_0);
  tmNode *n_1  = tmNode_create(mesh, xy_1);
  tmNode *n_2  = tmNode_create(mesh, xy_2);
  tmNode *n_3  = tmNode_create(mesh, xy_3);
  tmNode *n_4  = tmNode_create(mesh, xy_4);
  tmNode *n_5  = tmNode_create(mesh, xy_5);
  tmNode *n_6  = tmNode_create(mesh, xy_6);
  tmNode *n_7  = tmNode_create(mesh, xy_7);

  tmQtree_printQtree(mesh->nodes_qtree);

  tmMesh_destroy(mesh);

  return NULL;

} /* test_tmQtree_2() */

/*************************************************************
* Unit test function to handle the tmQtree structure
*
* Maximum number of qtree-objects must be set to 3  
* in order to get this test running
************************************************************/
char *test_tmQtree()
{
  tmDouble xy_min[2] = { -2.0, -2.0 };
  tmDouble xy_max[2] = {  2.0,  2.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 3, size_fun_1);

  /*--------------------------------------------------------
  | Add new nodes 
  --------------------------------------------------------*/
  tmDouble xy_1[2] = {-1.50,-1.50};
  tmDouble xy_2[2] = { 0.75,-0.75};
  tmDouble xy_3[2] = { 0.75, 1.25};
  tmDouble xy_4[2] = { 1.25, 0.75};
  tmDouble xy_5[2] = { 1.50, 1.50};
  tmDouble xy_6[2] = { 2.00, 1.50};
  tmDouble xy_7[2] = {-1.25,-1.00};
  tmDouble xy_8[2] = {-1.50,-0.50};
  tmDouble xy_9[2] = {-0.50,-0.50};
  tmDouble xy_10[2]= {-1.00, 1.00};

  tmNode *n_1 = tmNode_create(mesh, xy_1);
  tmNode *n_2 = tmNode_create(mesh, xy_2);
  tmNode *n_3 = tmNode_create(mesh, xy_3);

  mu_assert( tmQtree_containsObj(mesh->nodes_qtree, n_1, 1) == TRUE,
      "Node n_1 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree, n_2, 1) == TRUE,
      "Node n_2 has not been added to mesh qtree.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree, n_3, 1) == TRUE,
      "Node n_3 has not been added to mesh qtree.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == 3,
      "tmQtree_getObjNo returned wrong number of elements.");
  mu_assert(mesh->nodes_qtree->n_obj_tot == 3,
      "Failed to add node to mesh->nodes_qtree.");

  /*--------------------------------------------------------
  | Qtree should split when next node is added
  --------------------------------------------------------*/
  tmNode *n_4 = tmNode_create(mesh, xy_4);
  mu_assert( mesh->nodes_qtree->is_splitted == TRUE,
      "Qtree has not been splitted.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == 4,
      "tmQtree_getObjNo returned wrong number of elements.");

  mu_assert(mesh->nodes_qtree->n_obj_tot == 4,
      "Failed to split mesh->nodes_qtree.");
  mu_assert(mesh->nodes_qtree->n_obj == 0,
      "Failed to split mesh->nodes_qtree.");

  /*--------------------------------------------------------
  | Check if nodes are distributed correctly
  --------------------------------------------------------*/
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_1, 0) == TRUE,
      "Node n_1 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SE, n_1, 0) == FALSE,
      "Node n_1 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_1, 0) == FALSE,
      "Node n_1 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NW, n_1, 0) == FALSE,
      "Node n_1 has been distributed to a wrong child.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree->child_SW) == 1,
      "tmQtree_getObjNo returned wrong number of elements.");
  mu_assert(mesh->nodes_qtree->child_SW->n_obj_tot == 1,
      "Failed to add node to mesh->nodes_qtree.");

  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SE, n_2, 0) == TRUE,
      "Node n_2 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_2, 0) == FALSE,
      "Node n_2 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_2, 0) == FALSE,
      "Node n_2 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NW, n_2, 0) == FALSE,
      "Node n_2 has been distributed to a wrong child.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree->child_SE) == 1,
      "tmQtree_getObjNo returned wrong number of elements.");
  mu_assert(mesh->nodes_qtree->child_SE->n_obj_tot == 1,
      "Failed to add node to mesh->nodes_qtree.");

  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_3, 0) == TRUE,
      "Node n_3 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_4, 0) == TRUE,
      "Node n_4 has been distributed to a wrong child.");

  /*-------------------------------------------------------
  | Check that all nodes have been distributed from 
  | parent qtree to its childs
  -------------------------------------------------------*/
  ListNode *cur;
  tmBool obj_in_qtree = FALSE;
  int index = 0;
  for (cur = mesh->nodes_qtree->obj->first;
       cur != NULL; cur = cur->next)
  {
    if (cur->value == n_1)
      obj_in_qtree = TRUE;
    if (cur->value == n_2)
      obj_in_qtree = TRUE;
    if (cur->value == n_3)
      obj_in_qtree = TRUE;
    if (cur->value == n_4)
      obj_in_qtree = TRUE;
  }

  mu_assert( obj_in_qtree == FALSE,
      "Not all objects have been removed from the parent qtree.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree, n_1, 0) == FALSE,
      "Node n_1 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree, n_2, 0) == FALSE,
      "Node n_2 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree, n_3, 0) == FALSE,
      "Node n_3 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree, n_4, 0) == FALSE,
      "Node n_4 has not been removed from parent.");

  tmNode *n_5 = tmNode_create(mesh, xy_5);

  /*--------------------------------------------------------
  | Qtree->child_NE should split when next node is added
  --------------------------------------------------------*/
  tmNode *n_6 = tmNode_create(mesh, xy_6);
  mu_assert( mesh->nodes_qtree->child_NE->is_splitted == TRUE,
      "Qtree has not been splitted.");

  /*--------------------------------------------------------
  | Check that all nodes have been distributed from NE to
  | its children
  --------------------------------------------------------*/
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_3, 0) == FALSE,
      "Node n_3 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_4, 0) == FALSE,
      "Node n_4 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_5, 0) == FALSE,
      "Node n_5 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_6, 0) == FALSE,
      "Node n_6 has not been removed from parent.");

  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE->child_NW, n_3, 0) == TRUE,
      "Node n_3 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE->child_SE, n_4, 0) == TRUE,
      "Node n_4 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE->child_NE, n_5, 0) == TRUE,
      "Node n_5 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE->child_NE, n_6, 0) == TRUE,
      "Node n_6 has been distributed to a wrong child.");

  /*--------------------------------------------------------
  | Add further nodes
  --------------------------------------------------------*/
  tmNode *n_7 = tmNode_create(mesh, xy_7);
  tmNode *n_8 = tmNode_create(mesh, xy_8);

  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == 8,
      "tmQtree_getObjNo returned wrong number of elements.");

  /*--------------------------------------------------------
  | Qtree->child_SW should split when next node is added
  --------------------------------------------------------*/
  tmNode *n_9 = tmNode_create(mesh, xy_9);
  mu_assert( mesh->nodes_qtree->child_SW->is_splitted == TRUE,
      "Qtree has not been splitted.");

  /*--------------------------------------------------------
  | Check that all nodes have been distributed from SW to
  | its children
  --------------------------------------------------------*/
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_1, 0) == FALSE,
      "Node n_1 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_7, 0) == FALSE,
      "Node n_7 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_8, 0) == FALSE,
      "Node n_8 has not been removed from parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_9, 0) == FALSE,
      "Node n_9 has not been removed from parent.");

  tmNode *n_10= tmNode_create(mesh, xy_10); 
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == 10,
      "tmQtree_getObjNo returned wrong number of elements.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == mesh->nodes_qtree->n_obj_tot,
      "Failed to distribute number of nodes.");

  /*--------------------------------------------------------
  | Check function for finding objects in qree with bbox
  --------------------------------------------------------*/
  tmDouble bbox_min[2] = { -1.0, -1.0 };
  tmDouble bbox_max[2] = {  1.0,  1.0 };

  List *obj_bbox = tmQtree_getObjBbox(mesh->nodes_qtree, 
                                      bbox_min, bbox_max);

  mu_assert(obj_bbox->count == 3,
      "tmQtree_getObjBbox() failed.");
  mu_assert(obj_bbox->first->value == n_10,
      "tmQtree_getObjBbox() failed.");
  mu_assert(obj_bbox->first->next->value == n_9,
      "tmQtree_getObjBbox() failed.");
  mu_assert(obj_bbox->last->value == n_2,
      "tmQtree_getObjBbox() failed.");

  if (obj_bbox != NULL)
    List_destroy(obj_bbox);

  /*--------------------------------------------------------
  | Check function for finding objects in qree with circle
  --------------------------------------------------------*/
  tmDouble xy_c[2] = { 1.0, 1.0 };
  tmDouble r_c     = 0.75;

  List *obj_circ = tmQtree_getObjCirc(mesh->nodes_qtree, 
                                      xy_c, r_c);

  mu_assert(obj_circ->count == 3,
      "tmQtree_getObjCirc() failed.");
  mu_assert(obj_circ->first->value == n_5,
      "tmQtree_getObjCirc() failed.");
  mu_assert(obj_circ->first->next->value == n_3,
      "tmQtree_getObjCirc() failed.");
  mu_assert(obj_circ->last->value == n_4,
      "tmQtree_getObjCirc() failed.");

  if (obj_circ != NULL)
    List_destroy(obj_circ);



  /*--------------------------------------------------------
  | Remove node 1 -> child_SW must merge
  --------------------------------------------------------*/
  tmNode_destroy(n_1);
  mu_assert( mesh->nodes_qtree->is_splitted == TRUE,
      "Qtree is not splitted anymore.");
  mu_assert( mesh->nodes_qtree->child_SW->is_splitted == FALSE,
      "Qtree has not been merged.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_7, 0) == TRUE,
      "Node n_7 has not been put back to parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_8, 0) == TRUE,
      "Node n_8 has not been put back to parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_9, 0) == TRUE,
      "Node n_9 has not been put back to parent.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree->child_SW) == 3,
      "tmQtree_getObjNo returned wrong number of elements.");

  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == mesh->nodes_qtree->n_obj_tot,
      "Failed to distribute number of nodes.");

  /*--------------------------------------------------------
  | Remove node 6 -> child_NE must merge
  --------------------------------------------------------*/
  tmNode_destroy(n_6);
  mu_assert( mesh->nodes_qtree->is_splitted == TRUE,
      "Qtree is not splitted anymore.");
  mu_assert( mesh->nodes_qtree->child_NE->is_splitted == FALSE,
      "Qtree has not been merged.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_3, 0) == TRUE,
      "Node n_3 has not been put back to parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_4, 0) == TRUE,
      "Node n_4 has not been put back to parent.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_5, 0) == TRUE,
      "Node n_5 has not been put back to parent.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree->child_NE) == 3,
      "tmQtree_getObjNo returned wrong number of elements.");

  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == mesh->nodes_qtree->n_obj_tot,
      "Failed to distribute number of nodes.");

  tmMesh_destroy(mesh);

  return NULL;
} /* test_tmQtree() */


/*************************************************************
* Unit test function to handle the tmQtree performance
************************************************************/
char *test_tmQtree_performance()
{

  tmDouble xy_min[2] = { -50.0, -55.0 };
  tmDouble xy_max[2] = {  55.0,  50.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 10, size_fun_1);

  int n_nodes = 1000;

  tmDouble a = 2.5;
  tmDouble b =-3.4;
  tmDouble c = 6.5;
  tmDouble t_0 = 0.0;
  tmDouble t_1 = 5.0 * PI_D; 
  tmDouble dt  = (t_1 - t_0) / (tmDouble)n_nodes;

  int i;
  tmNode *n;
  tmDouble n_xy[2]     = { 0.0, 0.0 };
  tmDouble bbox_min[2] = { 0.0, 0.0 };
  tmDouble bbox_max[2] = { 0.0, 0.0 };
  tmDouble t;

  int n_obj_qt = 0;
  int n_obj_bf = 0;
  int n_total  = 0;

  clock_t tic_0 = clock();
  /*--------------------------------------------------------
  | Create nodes in domain
  --------------------------------------------------------*/
  for (i = 0; i < n_nodes; i++)
  {
    t = t_0 + dt * (tmDouble)i;
    n_xy[0] = (a + b * t) * cos(t) + c * sin(40. * t);
    n_xy[1] = (a + b * t) * sin(t) + c * cos(40. * t);
    n = tmNode_create(mesh, n_xy); 
  }

  /*--------------------------------------------------------
  | Print out qtree
  --------------------------------------------------------*/
  tmQtree_printQtree(mesh->nodes_qtree);

  clock_t tic_1 = clock();

  /*--------------------------------------------------------
  | Find objects within bbox of every node with qtree
  --------------------------------------------------------*/
  for (i = 0; i < n_nodes; i++)
  {
    t = t_0 + dt * (tmDouble)i;
    n_xy[0] = (a + b * t) * cos(t) + c * sin(40. * t);
    n_xy[1] = (a + b * t) * sin(t) + c * cos(40. * t);

    tmDouble bbox_min[2] = { n_xy[0]-2.0, n_xy[1]-2.0 };
    tmDouble bbox_max[2] = { n_xy[0]+2.0, n_xy[1]+2.0 };

    List *obj_bbox = tmQtree_getObjBbox(mesh->nodes_qtree, 
                                        bbox_min, bbox_max);


    if (obj_bbox != NULL)
    {
      n_obj_qt += obj_bbox->count;
      List_destroy(obj_bbox);
    }
    
  }

  clock_t tic_2 = clock();
  /*--------------------------------------------------------
  | Brute force search for objects in bbox
  --------------------------------------------------------*/
  ListNode *cur;
  tmDouble *cur_xy;
  tmBool    in_bbox;

  for (i = 0; i < n_nodes; i++)
  {
    t = t_0 + dt * (tmDouble)i;
    n_xy[0] = (a + b * t) * cos(t) + c * sin(40. * t);
    n_xy[1] = (a + b * t) * sin(t) + c * cos(40. * t);

    tmDouble bbox_min[2] = { n_xy[0]-2.0, n_xy[1]-2.0 };
    tmDouble bbox_max[2] = { n_xy[0]+2.0, n_xy[1]+2.0 };

    for (cur = mesh->nodes_stack->first;
        cur != NULL; cur = cur->next)
    {
      n_total += 1;
      cur_xy  = ((tmNode*)cur->value)->xy;
      in_bbox = IN_ON_BBOX(cur_xy, bbox_min, bbox_max);

      if (in_bbox == TRUE)
        n_obj_bf += 1;
    }
  }

  clock_t tic_3 = clock();
  tmMesh_destroy(mesh);

  clock_t tic_4 = clock();

  printf("-------------------------------------------------------------------\n");
  printf(" tmQtree Performance test                \n");
  printf("-------------------------------------------------------------------\n");
  printf(" > Number of brute-force calls: %d\n", n_total);
  printf(" > Found nodes in bbox: %d/%d\n", n_obj_qt, n_obj_bf);
  printf("-------------------------------------------------------------------\n");
  printf(" > Total time                    : %e sec\n", (double) (tic_4 - tic_0) / CLOCKS_PER_SEC );
  printf(" > Time for initilization        : %e sec\n", (double) (tic_1 - tic_0) / CLOCKS_PER_SEC );
  printf(" > Time for qtree search         : %e sec\n", (double) (tic_2 - tic_1) / CLOCKS_PER_SEC );
  printf(" > Time for brute force search   : %e sec\n", (double) (tic_3 - tic_2) / CLOCKS_PER_SEC );
  printf(" > Time to clear everything      : %e sec\n", (double) (tic_4 - tic_3) / CLOCKS_PER_SEC );
  printf("-------------------------------------------------------------------\n");

  return NULL;

} /* test_tmQtree_performance() */


/************************************************************
* Unit test function for the triangle intersection 
************************************************************/
char *test_tmTri_intersection()
{
  tmDouble xy_min[2] = {   0.0,  0.0 };
  tmDouble xy_max[2] = {  10.0, 10.0 };

  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 10, size_fun_3);

  /*--------------------------------------------------------
  | node definition
  --------------------------------------------------------*/
  tmDouble xy1[2]  = {  1.0,  1.0 };
  tmDouble xy2[2]  = {  4.0,  1.0 };
  tmDouble xy3[2]  = {  1.0,  4.0 };
  tmDouble xy4[2]  = {  4.0,  3.0 };
  tmDouble xy5[2]  = {  8.0,  3.0 };
  tmDouble xy6[2]  = {  3.0,  4.0 };
  tmDouble xy7[2]  = {  1.0,  7.0 };
  tmDouble xy8[2]  = {  4.0,  6.0 };
  tmDouble xy9[2]  = {  6.0,  6.0 };
  tmDouble xy10[2] = {  8.0,  6.0 };
  tmDouble xy11[2] = {  3.0,  9.0 };
  tmDouble xy12[2] = {  8.0,  9.0 };

  tmNode *n1  = tmNode_create(mesh, xy1);
  tmNode *n2  = tmNode_create(mesh, xy2);
  tmNode *n3  = tmNode_create(mesh, xy3);
  tmNode *n4  = tmNode_create(mesh, xy4);
  tmNode *n5  = tmNode_create(mesh, xy5);
  tmNode *n6  = tmNode_create(mesh, xy6);
  tmNode *n7  = tmNode_create(mesh, xy7);
  tmNode *n8  = tmNode_create(mesh, xy8);
  tmNode *n9  = tmNode_create(mesh, xy9);
  tmNode *n10 = tmNode_create(mesh, xy10);
  tmNode *n11 = tmNode_create(mesh, xy11);
  tmNode *n12 = tmNode_create(mesh, xy12);

  /*--------------------------------------------------------
  | Create triangles
  --------------------------------------------------------*/
  tmTri *t1 = tmTri_create(mesh, n1 , n2 , n3 );
  tmTri *t2 = tmTri_create(mesh, n2 , n8 , n3 );
  tmTri *t3 = tmTri_create(mesh, n2 , n5 , n4 );
  tmTri *t4 = tmTri_create(mesh, n4 , n5 , n9 );
  tmTri *t5 = tmTri_create(mesh, n8 , n10, n12);
  tmTri *t6 = tmTri_create(mesh, n6 , n11, n7 );

  /*--------------------------------------------------------
  | Check intersections
  --------------------------------------------------------*/
  mu_assert( tmTri_triIntersect(t1, t2) == FALSE,
      "tmTri_triIntersect() gives wrong results.");
  mu_assert( tmTri_triIntersect(t3, t4) == FALSE,
      "tmTri_triIntersect() gives wrong results.");
  mu_assert( tmTri_triIntersect(t2, t5) == FALSE,
      "tmTri_triIntersect() gives wrong results.");

  mu_assert( tmTri_triIntersect(t2, t3) == TRUE,
      "tmTri_triIntersect() gives wrong results.");
  mu_assert( tmTri_triIntersect(t2, t6) == TRUE,
      "tmTri_triIntersect() gives wrong results.");
  mu_assert( tmTri_triIntersect(t4, t5) == TRUE,
      "tmTri_triIntersect() gives wrong results.");

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);
  tmMesh_destroy(mesh);

  return NULL;

} /* test_tmTri_intersection() */


/*************************************************************
* Unit test function for the advancing front algorithm
************************************************************/
char *test_tmFront_init()
{
  tmDouble xy_min[2] = {  -5.0, -5.0 };
  tmDouble xy_max[2] = {  15.0, 15.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 3, size_fun_1);

  /*--------------------------------------------------------
  | exterior nodes
  --------------------------------------------------------*/
  tmDouble xy0[2] = {  0.0,  0.0 };
  tmDouble xy1[2] = {  8.0,  0.0 };
  tmDouble xy2[2] = {  9.0,  7.0 };
  tmDouble xy3[2] = {  0.0,  5.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);
  tmNode *n3 = tmNode_create(mesh, xy3);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);
  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n3, 0);
  tmEdge *e3 = tmBdry_edgeCreate(bdry_ext, n3, n0, 0);

  /*--------------------------------------------------------
  | Initialize the advancing front and 
  | check for its correct initialization 
  --------------------------------------------------------*/
  tmFront_init(mesh);

  ListNode *cur_front, *cur_bdry;
  ListNode *nxt_front, *nxt_bdry;
  cur_front = nxt_front = mesh->front->edges_stack->first;
  cur_bdry = nxt_bdry = bdry_ext->edges_stack->first;
  while (cur_front != NULL || cur_bdry != NULL)
  {
    nxt_front = cur_front->next;
    nxt_bdry = cur_bdry->next;
    mu_assert( ((tmEdge*)cur_front->value)->n1 == ((tmEdge*)cur_bdry->value)->n1,
        "Failed to init front.");
    mu_assert( ((tmEdge*)cur_front->value)->n2 == ((tmEdge*)cur_bdry->value)->n2,
        "Failed to init front.");
    cur_front = nxt_front;
    cur_bdry = nxt_bdry;
  }

  /*--------------------------------------------------------
  | Sort advancing front according to edge lengths
  | and check if sorting succeeded
  --------------------------------------------------------*/
  tmFront_sortEdges(mesh);

  cur_front = nxt_front = mesh->front->edges_stack->first;
  while (cur_front != NULL)
  {
    nxt_front = cur_front->next;
    if (nxt_front != NULL)
    {
      tmDouble cur_len = ((tmEdge*)cur_front->value)->len;
      tmDouble nxt_len = ((tmEdge*)nxt_front->value)->len;
      mu_assert(cur_len <= nxt_len,
          "Failed to sort the advancing front");
    }
    cur_front = nxt_front;
  }

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);

  tmMesh_destroy(mesh);

  return NULL;
} /* test_tmFront_init() */


/*************************************************************
* Unit test function for the advancing front algorithm
************************************************************/
char *test_tmFront_advance()
{
  tmDouble xy_min[2] = { -15.0,-15.0 };
  tmDouble xy_max[2] = {  15.0, 15.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 3, size_fun_2);

  /*--------------------------------------------------------
  | exterior nodes
  --------------------------------------------------------*/
  tmDouble xy0[2] = {  0.0,  0.0 };
  tmDouble xy1[2] = { 10.0,  0.0 };
  tmDouble xy2[2] = { 10.0, 10.0 };
  tmDouble xy3[2] = {  0.0, 10.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);
  tmNode *n3 = tmNode_create(mesh, xy3);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);
  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n3, 0);
  tmEdge *e3 = tmBdry_edgeCreate(bdry_ext, n3, n0, 0);

  /*--------------------------------------------------------
  | Refine whole boundary according to size function
  --------------------------------------------------------*/
  tmBdry_refine(bdry_ext);

  /*--------------------------------------------------------
  | Initialize the advancing front and 
  | check for its correct initialization 
  --------------------------------------------------------*/
  tmFront_init(mesh);
  tmFront_sortEdges(mesh);

  /*--------------------------------------------------------
  | Create new point at smallest edge
  --------------------------------------------------------*/
  tmEdge *ne = (tmEdge*) mesh->front->edges_stack->first->value;
  tmNode *nn = tmEdge_createNode(ne);

  /*--------------------------------------------------------
  | Get nodes in vicinity of nn
  --------------------------------------------------------*/
  List *nn_nb = tmNode_getNbrsFromSizeFun(nn);

  tmNode *first = (tmNode*) nn_nb->first->value;
  tmNode *last  = (tmNode*) nn_nb->last->value;

  mu_assert( first == nn, 
      "tmNode_getNbrsFromSizeFun() error.");
  mu_assert( nn_nb->count == 2,
      "tmNode_getNbrsFromSizeFun() error.");

  /*--------------------------------------------------------
  | Form potential triangle with found nodes 
  | -> Begin with closest
  --------------------------------------------------------*/
  tmNode *winner  = (tmNode*) nn_nb->first->next->value;
  tmTri  *nt      = tmTri_create(nn->mesh, 
                                 ne->n1, 
                                 ne->n2, 
                                 winner);

  mu_assert( EQ( 180.0 * nt->minAngle / PI_D, 45.0 ),
      "Resulting triangle is not right.")
  mu_assert( EQ( 180.0 * nt->maxAngle / PI_D, 90.0 ),
      "Resulting triangle is not right.")

  /*--------------------------------------------------------
  | Get node neighbors on advancing front of edge nodes
  --------------------------------------------------------*/
  List *n1_nb = tmNode_getFrontNbrs(ne->n1);
  List *n2_nb = tmNode_getFrontNbrs(ne->n2);

  tmEdge *n1_e = tmNode_getAdjFrontEdge(ne->n1, winner);
  tmEdge *n2_e = tmNode_getAdjFrontEdge(ne->n2, winner);

  /*--------------------------------------------------------
  | Get edges in front that also share nn
  --------------------------------------------------------*/
  if ( n1_e != NULL )
  {
    tmEdge_destroy(n1_e);
    tmFront_edgeCreate(mesh->front, winner, ne->n2, NULL);
  }

  if ( n2_e != NULL )
  {
    tmEdge_destroy(n2_e);
    tmFront_edgeCreate(mesh->front, ne->n1, winner, NULL);
  }

  if ( n1_e == NULL && n2_e == NULL )
  {
    tmFront_edgeCreate(mesh->front, ne->n1, winner, NULL);
    tmFront_edgeCreate(mesh->front, winner, ne->n2, NULL);
  }

  tmEdge_destroy(ne);


  if (nn_nb != NULL)
    List_destroy(nn_nb);
  if (n1_nb != NULL)
    List_destroy(n1_nb);
  if (n2_nb != NULL)
    List_destroy(n2_nb);

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  //tmMesh_printMesh(mesh);
  tmMesh_destroy(mesh);

  return NULL;

} /* test_tmFront_advance() */


/*************************************************************
* Unit test function for the advancing front algorithm
************************************************************/
char *test_tmFront_simpleMesh()
{
  tmDouble xy_min[2] = {  -0.0,  0.0 };
  tmDouble xy_max[2] = { 120.0, 20.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 10, size_fun_4);

  /*--------------------------------------------------------
  | exterior nodes
  --------------------------------------------------------*/
  tmDouble xy0[2] = {  0.0,  0.0 };
  tmDouble xy1[2] = { 20.0,  0.0 };
  tmDouble xy2[2] = { 20.0, 20.0 };
  tmDouble xy3[2] = {  0.0, 20.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);
  tmNode *n3 = tmNode_create(mesh, xy3);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);
  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n3, 0);
  tmEdge *e3 = tmBdry_edgeCreate(bdry_ext, n3, n0, 0);

  /*--------------------------------------------------------
  | Refine whole boundary according to size function
  --------------------------------------------------------*/
  tmBdry_refine(bdry_ext);

  /*--------------------------------------------------------
  | Create mesh
  --------------------------------------------------------*/
  clock_t tic_1 = clock();
  tmMesh_ADFMeshing(mesh);
  clock_t tic_2 = clock();

  printf("> ----------------------------------------------\n");
  printf("> simple mesh performance test                \n");
  printf("> ----------------------------------------------\n");
  printf("> Number of elements: %d\n", mesh->no_tris);
  printf("> Meshing time      : %e sec\n", (double) (tic_2 - tic_1) / CLOCKS_PER_SEC );
  printf("> ----------------------------------------------\n");


  /*--------------------------------------------------------
  | Flip edges which are not locally delaunay
  --------------------------------------------------------*/
  tmMesh_delaunayFlip(mesh);

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);
  tmMesh_destroy(mesh);


  return NULL;

} /* test_tmFront_simpleMesh() */

/************************************************************
* Unit test function for the advancing front algorithm
************************************************************/
char *test_tmFront_innerOuterMesh()
{
  tmDouble xy_min[2] = {  -0.0,  0.0 };
  tmDouble xy_max[2] = { 120.0,120.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 10, size_fun_4);

  /*--------------------------------------------------------
  | exterior nodes
  --------------------------------------------------------*/
  tmDouble xy0[2] = {  0.0,  0.0 };
  tmDouble xy1[2] = {120.0,  0.0 };
  tmDouble xy2[2] = {120.0,120.0 };
  tmDouble xy3[2] = {  0.0,120.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);
  tmNode *n3 = tmNode_create(mesh, xy3);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);
  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE, 1);

  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n3, 0);
  tmEdge *e3 = tmBdry_edgeCreate(bdry_ext, n3, n0, 0);

  /*--------------------------------------------------------
  | interior nodes
  --------------------------------------------------------*/
  tmDouble xy4[2]  = { 40.0, 100.0 };
  tmDouble xy5[2]  = { 90.0, 100.0 };
  tmDouble xy6[2]  = { 90.0,  85.0 };
  tmDouble xy7[2]  = { 60.0,  85.0 };
  tmDouble xy8[2]  = { 60.0,  65.0 };
  tmDouble xy9[2]  = { 80.0,  65.0 };
  tmDouble xy10[2] = { 80.0,  50.0 };
  tmDouble xy11[2] = { 60.0,  50.0 };
  tmDouble xy12[2] = { 60.0,  10.0 };
  tmDouble xy13[2] = { 40.0,  10.0 };

  tmNode *n4  = tmNode_create(mesh, xy4);
  tmNode *n5  = tmNode_create(mesh, xy5);
  tmNode *n6  = tmNode_create(mesh, xy6);
  tmNode *n7  = tmNode_create(mesh, xy7);
  tmNode *n8  = tmNode_create(mesh, xy8);
  tmNode *n9  = tmNode_create(mesh, xy9);
  tmNode *n10 = tmNode_create(mesh, xy10);
  tmNode *n11 = tmNode_create(mesh, xy11);
  tmNode *n12 = tmNode_create(mesh, xy12);
  tmNode *n13 = tmNode_create(mesh, xy13);

  tmEdge *e4  = tmBdry_edgeCreate(bdry_int, n4, n5, 1);
  tmEdge *e5  = tmBdry_edgeCreate(bdry_int, n5, n6, 1);
  tmEdge *e6  = tmBdry_edgeCreate(bdry_int, n6, n7, 1);
  tmEdge *e7  = tmBdry_edgeCreate(bdry_int, n7, n8, 1);
  tmEdge *e8  = tmBdry_edgeCreate(bdry_int, n8, n9, 1);
  tmEdge *e9  = tmBdry_edgeCreate(bdry_int, n9, n10, 1);
  tmEdge *e10 = tmBdry_edgeCreate(bdry_int, n10, n11, 1);
  tmEdge *e11 = tmBdry_edgeCreate(bdry_int, n11, n12, 1);
  tmEdge *e12 = tmBdry_edgeCreate(bdry_int, n12, n13, 1);
  tmEdge *e13 = tmBdry_edgeCreate(bdry_int, n13, n4, 1);

  /*--------------------------------------------------------
  | Refine whole boundary according to size function
  --------------------------------------------------------*/
  tmBdry_refine(bdry_ext);
  tmBdry_refine(bdry_int);

  /*--------------------------------------------------------
  | Create mesh
  --------------------------------------------------------*/
  tmMesh_ADFMeshing(mesh);

  /*--------------------------------------------------------
  | Flip edges which are not locally delaunay
  --------------------------------------------------------*/
  tmMesh_delaunayFlip(mesh);

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);

  tmMesh_destroy(mesh);

  return NULL;

} /* test_tmFront_innerOuterMesh() */

char *test_tmFront_simpleMesh2()
{
  tmDouble xy_min[2] = {-10.0,-10.0 };
  tmDouble xy_max[2] = { 20.0, 20.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 10, size_fun_6);

  /*--------------------------------------------------------
  | exterior boundary
  --------------------------------------------------------*/
  tmDouble xy0[2] = { -1.0, -1.0 };
  tmDouble xy1[2] = { 16.0,  7.0 };
  tmDouble xy2[2] = { -1.0, 10.0 };

  tmNode *n0 = tmNode_create(mesh, xy0);
  tmNode *n1 = tmNode_create(mesh, xy1);
  tmNode *n2 = tmNode_create(mesh, xy2);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);
  tmEdge *e0 = tmBdry_edgeCreate(bdry_ext, n0, n1, 0);
  tmEdge *e1 = tmBdry_edgeCreate(bdry_ext, n1, n2, 0);
  tmEdge *e2 = tmBdry_edgeCreate(bdry_ext, n2, n0, 0);

  /*--------------------------------------------------------
  | interior boundary
  --------------------------------------------------------*/
  tmDouble xy3[2] = {  1.0,  4.0 };
  tmDouble xy4[2] = {  6.0,  8.0 };
  tmDouble xy5[2] = {  6.0,  4.0 };

  tmNode *n3 = tmNode_create(mesh, xy3);
  tmNode *n4 = tmNode_create(mesh, xy4);
  tmNode *n5 = tmNode_create(mesh, xy5);

  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE, 1);
  tmEdge *e3 = tmBdry_edgeCreate(bdry_int, n3, n4, 1);
  tmEdge *e4 = tmBdry_edgeCreate(bdry_int, n4, n5, 1);
  tmEdge *e5 = tmBdry_edgeCreate(bdry_int, n5, n3, 1);

  /*--------------------------------------------------------
  | Refine whole boundary according to size function
  --------------------------------------------------------*/
  tmBdry_refine(bdry_ext);
  tmBdry_refine(bdry_int);

  /*--------------------------------------------------------
  | Create mesh
  --------------------------------------------------------*/
  clock_t tic_1 = clock();
  tmMesh_ADFMeshing(mesh);
  clock_t tic_2 = clock();

  printf("> ----------------------------------------------\n");
  printf("> simple mesh 2 performance test                \n");
  printf("> ----------------------------------------------\n");
  printf("> Number of elements: %d\n", mesh->no_tris);
  printf("> Meshing time      : %e sec\n", (double) (tic_2 - tic_1) / CLOCKS_PER_SEC );
  printf("> ----------------------------------------------\n");


  /*--------------------------------------------------------
  | Flip edges which are not locally delaunay
  --------------------------------------------------------*/
  tmMesh_delaunayFlip(mesh);

  /*--------------------------------------------------------
  | Test specific functions
  --------------------------------------------------------*
  tmDouble xy_t1[2] = { 2.0, 5.0 };
  tmTri *t1 = tmMesh_getTriFromCoords(mesh, xy_t1);
  mu_assert( t1->index == 21,
      "tmMesh_getTriFromCoords() failed.");

  t1 = tmMesh_getTriFromCoords(mesh, xy0);
  mu_assert( t1->index == 10,
      "tmMesh_getTriFromCoords() failed.");

  t1 = tmMesh_getTriFromCoords(mesh, xy3);
  mu_assert( t1->index == 21,
      "tmMesh_getTriFromCoords() failed.");*/

  //tmDouble xy_refine[2] = { 0.5, 5.0 };
  //tmMesh_refineLocally(mesh, xy_refine);

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);


  tmMesh_destroy(mesh);

  return NULL;

} /* test_tmFront_simpleMesh2() */
