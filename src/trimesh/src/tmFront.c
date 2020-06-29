#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmBdry.h"
#include "trimesh/tmQtree.h"
#include "trimesh/tmFront.h"
#include "trimesh/tmTri.h"

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
* @param front: advancing front structure to add edge to
* @param n1,n2: start / ending node defining the edge
* @param t:     triangle that is located to the right
*               of the new edge
* 
**********************************************************/
tmEdge *tmFront_addEdge(tmFront *front, 
                        tmNode  *n1, 
                        tmNode  *n2,
                        tmTri   *t)
{

  tmEdge *edge = tmEdge_create(front->mesh, n1, n2, NULL, 1);

  front->edges_head = edge;
  front->no_edges += 1;

  List_push(front->edges_stack, edge);
  tmQtree_addObj(front->edges_qtree, edge);
  edge->stack_pos = List_last_node(front->edges_stack);

  /*--------------------------------------------------------
  | Set triangle that is located to the right of the new 
  | edge
  --------------------------------------------------------*/
  edge->t2 = t;

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
    log_warn("Can not remove edge from front. Edge not found.");

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
  | Destroy edge -> removes also adjacency to edge nodes
  -------------------------------------------------------*/
  tmEdge_destroy(edge);

} /* tmFront_remEdge() */

/**********************************************************
* Function: tmFront_init()
*----------------------------------------------------------
* Function to initialize the advancing front of a mesh
* --> Clone of all current boundaries
*----------------------------------------------------------
* 
**********************************************************/
void tmFront_init(tmMesh *mesh)
{
  ListNode *cur, *cur_bdry;

  tmFront *front = mesh->front;

  /*-------------------------------------------------------
  | Loop over all boundaries
  -------------------------------------------------------*/
  for (cur_bdry = mesh->bdry_stack->first; 
       cur_bdry != NULL; cur_bdry = cur_bdry->next)
  {
    tmBdry *bdry = (tmBdry*)cur_bdry->value;

    /*-----------------------------------------------------
    | Loop over all boundary edges
    -----------------------------------------------------*/
    for (cur = bdry->edges_stack->first; 
         cur != NULL; cur = cur->next)
    {
      tmNode *n1 = ((tmEdge*)cur->value)->n1;
      tmNode *n2 = ((tmEdge*)cur->value)->n2;

      tmFront_addEdge(front, n1, n2, NULL);
    }
  }

} /* tmFront_init() */

/**********************************************************
* Function: tmFront_sortEdges()
*----------------------------------------------------------
* Function to sort the advancing front edges 
* according to their length in ascending order 
*----------------------------------------------------------
* 
**********************************************************/
void tmFront_sortEdges(tmMesh *mesh)
{
  ListNode *cur;
  tmFront  *front       = mesh->front;
  List     *front_edges = front->edges_stack; 

  /*-------------------------------------------------------
  | Sort edge entries 
  | -> Only list values are swapped
  -------------------------------------------------------*/
  int sorted = List_bubble_sort(front_edges, 
                        (List_compare) tmEdge_compareLen);
  check( sorted == 0,
      "List sort in tmFront_sortEdges() failed.");

  /*-------------------------------------------------------
  | Update list positions 
  -------------------------------------------------------*/
  for (cur = front_edges->first; 
       cur != NULL; cur = cur->next)
  {
    ((tmEdge*)cur->value)->stack_pos = cur;
  }


error:
  return;

} /* tmFront_sortEdges() */

/**********************************************************
* Function: tmFront_advance()
*----------------------------------------------------------
* Function to advance the front edges 
* by one step
*----------------------------------------------------------
* @parameter mesh:  pointer to mesh structure
* @parameter eStart: pointer to advancing front edge
*                    which will be advanced
* @return boolean:  if advancement was successfull
* 
**********************************************************/
tmBool tmFront_advance(tmMesh *mesh, tmEdge *e_ad)
{
  tmNode   *cn;
  tmTri    *nt;
  ListNode *cur, *nxt;

  /*--------------------------------------------------------
  | Create new point at edge
  | Node is set inactive until it is needed to form a
  | triangle
  --------------------------------------------------------*/
  tmNode *nn    = tmEdge_createNode(e_ad);
  nn->is_active = FALSE;

#if (TM_DEBUG > 1)
  tmPrint("NEW NODE %d: (%.3f, %.3f)",
      nn->index, nn->xy[0], nn->xy[1]);
#endif

  /*--------------------------------------------------------
  | Get nodes in vicinity of nn
  --------------------------------------------------------*/
  List *nn_nb = tmNode_getNbrsFromSizeFun(nn);

  if (nn_nb != NULL)
  {
    /*------------------------------------------------------
    | Form potential triangle with found nodes 
    | -> Begin with closest
    ------------------------------------------------------*/
    cur = nn_nb->first;

    int iter = 0;
    while (cur != NULL)
    {
      iter += 1;
#if (TM_DEBUG > 1)
      tmPrint("CHECKING POT. NEIGHBOR: %d/%d - NODE %d", 
          iter, nn_nb->count,
          ((tmNode*)cur->value)->index);
#endif

      nxt = cur->next;
      cn  = (tmNode*)cur->value;

      /*----------------------------------------------------
      | Continue if node is not part of the front or if it 
      | the newly created node
      ----------------------------------------------------*/
      if ( cn->on_front == FALSE || cn == nn )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: NEIGHBOR NOT ON FRONT");
#endif
        cur = nxt;
        continue;
      }

      /*----------------------------------------------------
      | Continue if node is colinear to base edge
      ----------------------------------------------------*/
      if (ORIENTATION(e_ad->n1->xy,e_ad->n2->xy,cn->xy) == 0) 
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: NEIGHBOR IS COLLINEAR");
#endif
        cur = nxt;
        continue;
      }

      /*----------------------------------------------------
      | Create new potential triangle
      ----------------------------------------------------*/
      nt  = tmTri_create(mesh, e_ad->n1, e_ad->n2, cn);

      /*----------------------------------------------------
      | Continue if triangle is valid
      | Update advancing front with this node
      ----------------------------------------------------*/
      if ( tmTri_isValid(nt) == TRUE ) 
      {
        tmFront_update(mesh, cn, e_ad, nt);

        tmNode_destroy(nn);

        if (nn_nb != NULL)
          List_destroy(nn_nb);

#if (TM_DEBUG > 1)
        tmPrint(" -> NEW TRIANGLE %d: (%d, %d, %d)",
            nt->index,
            nt->n1->index, nt->n2->index, nt->n3->index);
#endif

        return TRUE;
      }

      /*----------------------------------------------------
      | If triangle is invalid, remove it 
      ----------------------------------------------------*/
      tmTri_destroy(nt);
      cur = nxt;

    }

    if (nn_nb != NULL)
      List_destroy(nn_nb);

  } /* if (nn_nb != NULL) */


#if (TM_DEBUG > 1)
  tmPrint("CHECKING NEW-NODE-TRIANGLE");  
#endif

  /*--------------------------------------------------------
  | New node is now set as active
  | 
  | Check if new node is not placed too close to any 
  | existing edges 
  | 
  | Form potential triangle with new node
  --------------------------------------------------------*/
  nn->is_active = TRUE;

  if ( tmNode_isValid(nn) == TRUE )
  {
    nt = tmTri_create(mesh, e_ad->n1, e_ad->n2, nn);

    if ( tmTri_isValid(nt) == TRUE ) 
    {
      tmFront_update(mesh, nn, e_ad, nt);

#if (TM_DEBUG > 1)
      tmPrint(" -> NEW TRIANGLE %d: (%d, %d, %d)",
          nt->index,
          nt->n1->index, nt->n2->index, nt->n3->index);
#endif

      return TRUE;
    }

    tmTri_destroy(nt);
  }

  tmNode_destroy(nn);

#if (TM_DEBUG > 1)
  tmPrint("FRONT-EDGE: (%d -> %d) FAILED", 
      e_ad->n1->index, e_ad->n2->index);
#endif

  return FALSE;

} /* tmFront_advance() */

/**********************************************************
* Function: tmFront_update()
*----------------------------------------------------------
* Function to update the front edges with a new node
*----------------------------------------------------------
* @param n: new node which is included in the front
* @param e: current edge which will be replaced
* 
**********************************************************/
void tmFront_update(tmMesh *mesh, 
                    tmNode *n, 
                    tmEdge *e, 
                    tmTri  *t)
{
  /*--------------------------------------------------------
  | Get advancing front edges that are adjacent to n
  --------------------------------------------------------*/
  tmEdge *e_n1 = tmNode_getAdjFrontEdge(e->n1, n);
  tmEdge *e_n2 = tmNode_getAdjFrontEdge(e->n2, n);

  /*--------------------------------------------------------
  | Both edge nodes are connected to new node
  | -> don't create new edge
  --------------------------------------------------------*/
  if ( e_n1 != NULL && e_n2 != NULL )
  {
    /*------------------------------------------------------
    | Pass edges to mesh
    ------------------------------------------------------*/
    tmMesh_addEdge(mesh, e_n1->n1, e_n1->n2, t, e_n1->t2);
    tmMesh_addEdge(mesh, e_n2->n1, e_n2->n2, t, e_n2->t2);

    /*------------------------------------------------------
    | Remove edges from front 
    ------------------------------------------------------*/
    tmFront_remEdge(mesh->front, e_n1);
    tmFront_remEdge(mesh->front, e_n2);
  }

  /*--------------------------------------------------------
  | First edge node is connected to new node
  | -> Create new edge between last edge node and new node
  --------------------------------------------------------*/
  if ( e_n1 != NULL && e_n2 == NULL )
  {
    /*------------------------------------------------------
    | Pass edge to mesh
    ------------------------------------------------------*/
    tmMesh_addEdge(mesh, e_n1->n1, e_n1->n2, t, e_n1->t2);

    /*------------------------------------------------------
    | Remove edge from front and create new front edge
    | t is right triangle of new front edge
    ------------------------------------------------------*/
    tmFront_remEdge(mesh->front, e_n1);
    tmFront_addEdge(mesh->front, n, e->n2, t);

  }

  /*--------------------------------------------------------
  | Last edge node is connected to new node
  | -> Create new edge between first edge node and new node
  --------------------------------------------------------*/
  if ( e_n1 == NULL && e_n2 != NULL )
  {
    /*------------------------------------------------------
    | Pass edge to mesh
    ------------------------------------------------------*/
    tmMesh_addEdge(mesh, e_n2->n1, e_n2->n2, t, e_n2->t2);

    /*------------------------------------------------------
    | Remove edge from front and create new front edge
    | t is right triangle of new front edge
    ------------------------------------------------------*/
    tmFront_remEdge(mesh->front, e_n2);
    tmFront_addEdge(mesh->front, e->n1, n, t);
  }

  /*--------------------------------------------------------
  | No edge node is connected to new node
  | -> Create edges between both edge nodes and new node
  --------------------------------------------------------*/
  if ( e_n1 == NULL && e_n2 == NULL )
  {
    tmFront_addEdge(mesh->front, n, e->n2, t);
    tmFront_addEdge(mesh->front, e->n1, n, t);
  }

  /*--------------------------------------------------------
  | Pass base edge to mesh 
  | then remove base edge from the front
  --------------------------------------------------------*/
  tmMesh_addEdge(mesh, e->n1, e->n2, t, e->t2);
  tmFront_remEdge(mesh->front, e);

} /* tmFront_update() */

