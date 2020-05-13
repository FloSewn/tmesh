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


/*************************************************************
* Unit test function to handle creation and 
* destruction of a trimesh structure
*************************************************************/
char *test_mesh_create_destroy()
{
  tmDouble xy_min[2] = { -2.0, -2.0 };
  tmDouble xy_max[2] = {  2.0,  2.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max);

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
  mu_assert( n_1->index == 0,
      "Failed to add node to mesh->nodes_stack.");
  mu_assert( n_2->index == 1,
      "Failed to add node to mesh->nodes_stack.");
  mu_assert( mesh->nodes_index == 3,
      "Failed to add node to mesh->nodes_stack.");

  mu_assert(List_first(mesh->nodes_qtree->obj) == n_1,
      "Failed to add node to mesh->nodes_qtree.");
  mu_assert(List_last(mesh->nodes_qtree->obj) == n_3,
      "Failed to add node to mesh->nodes_qtree.");
  mu_assert(mesh->nodes_qtree->n_obj == 3,
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
  mu_assert( e_1->index == 0,
      "Failed to add edge to mesh->edges_stack.");
  mu_assert( e_2->index == 1,
      "Failed to add edge to mesh->edges_stack.");
  mu_assert( mesh->edges_index == 2,
      "Failed to add edge to mesh->edges_stack.");

  /*--------------------------------------------------------
  | Add new triangle 
  --------------------------------------------------------*/
  tmTri *t_1 = tmTri_create(mesh, n_1, n_2, n_3);
  mu_assert( List_first(mesh->tris_stack) == t_1,
      "Failed to add tri to mesh->tris_stack.");
  mu_assert( t_1->index == 0,
      "Failed to add tri to mesh->tris_stack.");
  mu_assert( mesh->tris_index == 1,
      "Failed to add tri to mesh->tris_stack.");

  tmMesh_destroy(mesh);

  return NULL;
} /* test_mesh_create_destroy() */



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
char *test_tmQtree()
{

  tmDouble xy_min[2] = { -2.0, -2.0 };
  tmDouble xy_max[2] = {  2.0,  2.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max);

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

  /*--------------------------------------------------------
  | Qtree should split when next node is added
  --------------------------------------------------------*/
  tmNode *n_4 = tmNode_create(mesh, xy_4);
  mu_assert( mesh->nodes_qtree->is_splitted == TRUE,
      "Qtree has not been splitted.");
  mu_assert( tmQtree_getObjNo(mesh->nodes_qtree) == 4,
      "tmQtree_getObjNo returned wrong number of elements.");

  /*--------------------------------------------------------
  | Check if nodes are distributed correctly
  --------------------------------------------------------*/
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SW, n_1, 0) == TRUE,
      "Node n_1 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SE, n_1, 0) == FALSE,
      "Node n_1 has been distributed to a wrong child.");

  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_SE, n_2, 0) == TRUE,
      "Node n_2 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_3, 0) == TRUE,
      "Node n_3 has been distributed to a wrong child.");
  mu_assert( tmQtree_containsObj(mesh->nodes_qtree->child_NE, n_4, 0) == TRUE,
      "Node n_4 has been distributed to a wrong child.");

  /*-------------------------------------------------------
  | Check if object is in this qtree
  -------------------------------------------------------*/
  ListNode *cur;
  tmBool obj_in_qtree = FALSE;
  for (cur = mesh->nodes_qtree->obj->first;
       cur != NULL; cur = cur->next)
  {
    if (cur->value == n_1)
    {
      printf("n_1 in mesh->nodes_qtree:  INDEX: %d\n", ((tmNode*)cur->value)->index);
      obj_in_qtree = TRUE;
    }
    if (cur->value == n_2)
    {
      printf("n_2 in mesh->nodes_qtree:  INDEX: %d\n", ((tmNode*)cur->value)->index);
      obj_in_qtree = TRUE;
    }
    if (cur->value == n_3)
    {
      printf("n_3 in mesh->nodes_qtree:  INDEX: %d\n", ((tmNode*)cur->value)->index);
      obj_in_qtree = TRUE;
    }
    if (cur->value == n_4)
    {
      printf("n_4 in mesh->nodes_qtree:  INDEX: %d\n", ((tmNode*)cur->value)->index);
      obj_in_qtree = TRUE;
    }
  }
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

  tmNode *n_10= tmNode_create(mesh, xy_10); 

  /*--------------------------------------------------------
  | Remove node 1 -> child_SW must merge
  --------------------------------------------------------*/
  tmNode_destroy(n_1);
  mu_assert( mesh->nodes_qtree->is_splitted == TRUE,
      "Qtree is not splitted anymore.");
  mu_assert( mesh->nodes_qtree->child_SW->is_splitted == FALSE,
      "Qtree has not been merged.");

  /*--------------------------------------------------------
  | Remove node 6 -> child_NE must merge
  --------------------------------------------------------*/
  tmNode_destroy(n_6);
  mu_assert( mesh->nodes_qtree->is_splitted == TRUE,
      "Qtree is not splitted anymore.");
  mu_assert( mesh->nodes_qtree->child_NE->is_splitted == FALSE,
      "Qtree has not been merged.");

  tmMesh_destroy(mesh);

  return NULL;
} /* test_tmQtree() */
