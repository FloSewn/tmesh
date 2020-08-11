/*
 * This source file is part of the tmesh library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#include "tmesh/tmTypedefs.h"
#include "tmesh/tmList.h"
#include "tmesh/tmMesh.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmNode.h"
#include "tmesh/tmBdry.h"
#include "tmesh/tmQtree.h"
#include "tmesh/tmFront.h"
#include "tmesh/tmTri.h"

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
  front->edges_stack = tmList_create();
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
  tmListNode *cur, *nxt;

  /*-------------------------------------------------------
  | Free all front edges on the stack
  -------------------------------------------------------*/
  cur = nxt = front->edges_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmEdge_destroy(cur->value);
    cur = nxt;
  }

  /*-------------------------------------------------------
  | Destroy edges qtree and edges stack
  -------------------------------------------------------*/
  tmQtree_destroy(front->edges_qtree);
  tmList_destroy(front->edges_stack);

  /*-------------------------------------------------------
  | Finally free the front
  -------------------------------------------------------*/
  free(front);

} /* tmFront_destroy() */

/**********************************************************
* Function: tmFront_edgeCreate()
*----------------------------------------------------------
* Function to create a new edge on the advancing front
*----------------------------------------------------------
* @param front: advancing front structure to add edge to
* @param n1,n2: start / ending node defining the edge
* @param t:     triangle that is located to the right
*               of the new edge
* 
**********************************************************/
tmEdge *tmFront_edgeCreate(tmFront *front, 
                           tmNode  *n1, 
                           tmNode  *n2,
                           tmTri   *t)
{
  tmEdge *edge = tmEdge_create(front->mesh, n1, n2, 
                               NULL, 1);

  /*--------------------------------------------------------
  | Set triangle that is located to the right of the new 
  | edge
  --------------------------------------------------------*/
  edge->t2 = t;

  return edge;

} /* tmFront_edgeCreate() */

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
tmListNode *tmFront_addEdge(tmFront *front, tmEdge *edge)
{
  tmListNode *edge_pos;

  front->edges_head = edge;
  front->no_edges += 1;

  tmList_push(front->edges_stack, edge);
  tmQtree_addObj(front->edges_qtree, edge);
  edge_pos = tmList_last_node(front->edges_stack);

  return edge_pos;

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
  tmList_remove(front->edges_stack, edge->stack_pos);

  /*-------------------------------------------------------
  | Destroy edge -> removes also adjacency to edge nodes
  -------------------------------------------------------*/
  //tmEdge_destroy(edge);

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
  tmListNode *cur, *cur_bdry;

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

      tmFront_edgeCreate(front, n1, n2, NULL);
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
  tmListNode *cur;
  tmFront  *front       = mesh->front;
  tmList     *front_edges = front->edges_stack; 

  /*-------------------------------------------------------
  | Sort edge entries 
  | -> Only list values are swapped
  -------------------------------------------------------*/
  tmList *edges_sorted = tmList_merge_sort(front_edges,
                        (tmList_compare) tmEdge_compareLen);
  check( edges_sorted != NULL,
      "tmList sort in tmFront_sortEdges() failed.");

  if (front_edges != edges_sorted)
  {
    tmList_destroy(front_edges);
    front->edges_stack = edges_sorted;
    front_edges = edges_sorted;
  } 

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
  tmListNode *cur, *nxt;

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
  tmList *nn_nb = tmNode_getNbrsFromSizeFun(nn);

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
          tmList_destroy(nn_nb);

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
      tmList_destroy(nn_nb);

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
    tmMesh_edgeCreate(mesh, e_n1->n1, e_n1->n2, t, e_n1->t2);
    tmMesh_edgeCreate(mesh, e_n2->n1, e_n2->n2, t, e_n2->t2);

    /*------------------------------------------------------
    | Remove edges from front 
    ------------------------------------------------------*/
    tmEdge_destroy(e_n1);
    tmEdge_destroy(e_n2);
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
    tmMesh_edgeCreate(mesh, e_n1->n1, e_n1->n2, t, e_n1->t2);

    /*------------------------------------------------------
    | Remove edge from front and create new front edge
    | t is right triangle of new front edge
    ------------------------------------------------------*/
    tmEdge_destroy(e_n1);
    tmFront_edgeCreate(mesh->front, n, e->n2, t);

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
    tmMesh_edgeCreate(mesh, e_n2->n1, e_n2->n2, t, e_n2->t2);

    /*------------------------------------------------------
    | Remove edge from front and create new front edge
    | t is right triangle of new front edge
    ------------------------------------------------------*/
    tmEdge_destroy(e_n2);
    tmFront_edgeCreate(mesh->front, e->n1, n, t);
  }

  /*--------------------------------------------------------
  | No edge node is connected to new node
  | -> Create edges between both edge nodes and new node
  --------------------------------------------------------*/
  if ( e_n1 == NULL && e_n2 == NULL )
  {
    tmFront_edgeCreate(mesh->front, n, e->n2, t);
    tmFront_edgeCreate(mesh->front, e->n1, n, t);
  }

  /*--------------------------------------------------------
  | Pass base edge to mesh 
  | then remove base edge from the front
  --------------------------------------------------------*/
  tmMesh_edgeCreate(mesh, e->n1, e->n2, t, e->t2);
  tmEdge_destroy(e);

} /* tmFront_update() */


/**********************************************************
* Function: tmFront_refine()
*----------------------------------------------------------
* Refine the edges of the advancing front structure 
* according to a size function
*----------------------------------------------------------
* @param *mesh: pointer to mesh containing the front
* @return: 
**********************************************************/
void tmFront_refine(tmMesh *mesh)
{
  tmListNode *cur, *nxt;
  int counter = 0;

  tmFront *front = mesh->front;

  cur = nxt = front->edges_stack->first;

  tmSizeFun sizeFun = front->mesh->sizeFun;

  while ( counter < front->no_edges )
  {
    nxt = cur->next;

    tmDouble *xy_1 = ((tmEdge*)cur->value)->n1->xy;
    tmDouble *xy_m = ((tmEdge*)cur->value)->xy;

    tmDouble rho_1 = sizeFun(mesh, xy_1 );
    tmDouble rho_m = sizeFun(mesh, xy_m );
    tmDouble rho   = TM_FRONT_REFINE_FAC * (rho_1 + rho_m);
    check( rho > TM_MIN_SIZE,
        "Size function return value lower than defined minimum scale.");

    if ( ((tmEdge*)cur->value)->len > rho )
      tmFront_splitEdge(front, cur->value);
    else
      counter += 1;

    cur = nxt;

    if (cur == NULL)
      cur = front->edges_stack->first;
  }

error:
  return;

} /* tmFront_refine() */

/**********************************************************
* Function: tmFront_splitEdge()
*----------------------------------------------------------
* Split an edge by inserting a new node on its centroid
* and splitting it into two new edges
* This edge will be pointing from its node n1 to the new
* node and a new edge will be created, which points
* from the new node to n2.
*----------------------------------------------------------
* @param *front: pointer to front
* @param *edge: pointer to a tmEdge 
* @return: pointer to newly created first edge
**********************************************************/
tmEdge *tmFront_splitEdge(tmFront *front, tmEdge *edge)
{
  tmNode *n1 = edge->n1;
  tmNode *n2 = edge->n2;

  /*-------------------------------------------------------
  | Create new node and edges
  -------------------------------------------------------*/
  tmNode *nn = tmNode_create(edge->mesh, edge->xy);

  tmEdge_destroy(edge);

  tmEdge *ne1 = tmFront_edgeCreate(front, n1, nn, NULL);
  tmEdge *ne2 = tmFront_edgeCreate(front, nn, n2, NULL);

  return ne1;

} /* tmFront_splitEdge() */
