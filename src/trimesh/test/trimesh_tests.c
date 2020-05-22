#include "trimesh/tmNode.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmTri.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmQtree.h"

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
* Unit test function to handle creation and 
* destruction of a trimesh structure
*************************************************************/
char *test_mesh_create_destroy()
{
  tmDouble xy_min[2] = { -2.0, -2.0 };
  tmDouble xy_max[2] = {  2.0,  2.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 3);

  /*--------------------------------------------------------
  | Add new nodes 
  --------------------------------------------------------*/
  tmDouble xy_1[2] = {-0.0, 1.0 };
  tmDouble xy_2[2] = { 1.0, 0.0 };
  tmDouble xy_3[2] = { 1.0, 1.0 };

  tmNode *n_1 = tmNode_create(mesh, xy_1);
  tmNode *n_2 = tmNode_create(mesh, xy_2);
  tmNode *n_3 = tmNode_create(mesh, xy_3);

  mu_assert( List_first(mesh->nodes_stack) == n_1,
      "Failed to add node to mesh->nodes_stack.");
  mu_assert( List_last(mesh->nodes_stack) == n_3,
      "Failed to add node to mesh->nodes_stack.");

  mu_assert(List_first(mesh->nodes_qtree->obj) == n_1,
      "Failed to add node to mesh->nodes_qtree.");
  mu_assert(List_last(mesh->nodes_qtree->obj) == n_3,
      "Failed to add node to mesh->nodes_qtree.");
  mu_assert(mesh->nodes_qtree->n_obj == 3,
      "Failed to add node to mesh->nodes_qtree.");
  mu_assert(mesh->nodes_qtree->n_obj_tot == 3,
      "Failed to add node to mesh->nodes_qtree.");

  /*--------------------------------------------------------
  | Add new edges 
  --------------------------------------------------------*/
  tmEdge *e_1 = tmEdge_create(mesh, n_1, n_2);
  tmEdge *e_2 = tmEdge_create(mesh, n_2, n_3);

  mu_assert( List_first(mesh->edges_stack) == e_1,
      "Failed to add edge to mesh->edges_stack.");
  mu_assert( List_last(mesh->edges_stack) == e_2,
      "Failed to add edge to mesh->edges_stack.");

  /*--------------------------------------------------------
  | Add new triangle 
  --------------------------------------------------------*/
  tmTri *t_1 = tmTri_create(mesh, n_1, n_2, n_3);
  mu_assert( List_first(mesh->tris_stack) == t_1,
      "Failed to add tri to mesh->tris_stack.");

  tmMesh_destroy(mesh);

  return NULL;
} /* test_mesh_create_destroy() */



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
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 3);

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
  | Check function for finding objects in qree
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
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 10);

  int n_nodes = 100;

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
