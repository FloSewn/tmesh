#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmBdry.h"
#include "trimesh/tmQtree.h"
#include "trimesh/tmFront.h"

/**********************************************************
* Function: tmFront_create()
*----------------------------------------------------------
* Create a new tmFront structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmFront structure
**********************************************************/
tmFront *tmFront_create(tmMesh *mesh)
{
  tmFront *front = (tmFront*) calloc( 1, sizeof(tmFront) );
  check_mem(front);

  /*-------------------------------------------------------
  | Properties of this front
  -------------------------------------------------------*/
  front->mesh       = mesh;
  front->no_edges   = 0;

  /*-------------------------------------------------------
  | Advancing front edges
  -------------------------------------------------------*/
  front->edges_head  = NULL;
  front->edges_stack = List_create();
  front->edges_qtree = tmQtree_create(mesh, TM_EDGE);
  tmQtree_init(front->edges_qtree, NULL, 0, 
               mesh->xy_min, mesh->xy_max);

  return front;

error:
  return NULL;

} /* tmFront_create() */

/**********************************************************
* Function: tmFront_destroy()
*----------------------------------------------------------
* Destroys a tmFront structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmFront to destroy
*
**********************************************************/
void tmFront_destroy(tmFront *front)
{
  ListNode *cur, *nxt;

  /*-------------------------------------------------------
  | Free all front edges on the stack
  -------------------------------------------------------*/
  cur = nxt = front->edges_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmFront_remEdge(front, cur->value);
    cur = nxt;
  }

  /*-------------------------------------------------------
  | Destroy edges qtree and edges stack
  -------------------------------------------------------*/
  tmQtree_destroy(front->edges_qtree);
  List_destroy(front->edges_stack);

  /*-------------------------------------------------------
  | Finally free the front
  -------------------------------------------------------*/
  free(front);

} /* tmFront_destroy() */


/**********************************************************
* Function: tmFront_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmFront structure
* This edge is the new head of the front structure
*----------------------------------------------------------
* 
**********************************************************/
tmEdge *tmFront_addEdge(tmFront *front, 
                        tmNode  *n1, 
                        tmNode  *n2)
{

  tmEdge *edge = tmEdge_create( front->mesh, n1, n2, FALSE );

  front->edges_head = edge;
  front->no_edges += 1;

  List_push(front->edges_stack, edge);
  tmQtree_addObj(front->edges_qtree, edge);
  edge->stack_pos = List_last_node(front->edges_stack);

  return edge;

} /* tmFront_addEdge() */

/**********************************************************
* Function: tmFront_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmFront structure
*----------------------------------------------------------
*
**********************************************************/
void tmFront_remEdge(tmFront *front, tmEdge *edge)
{
  tmBool    qtree_rem;

  /*-------------------------------------------------------
  | Check if object is in the mesh
  -------------------------------------------------------*/
  if ( edge->front != front )
    log_warn("Can not remove edge from mesh. Edge not found.");

  /*-------------------------------------------------------
  | Remove edge from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(front->edges_qtree, edge);
  front->no_edges -= 1;

  /*-------------------------------------------------------
  | Remove edge from stack
  -------------------------------------------------------*/
  List_remove(front->edges_stack, edge->stack_pos);

  /*-------------------------------------------------------
  | Destroy edge
  -------------------------------------------------------*/
  tmEdge_destroy(edge);

} /* tmFront_remEdge() */
