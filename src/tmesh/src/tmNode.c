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
#include "tmesh/tmNode.h"
#include "tmesh/tmTri.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmQtree.h"
#include "tmesh/tmBdry.h"
#include "tmesh/tmFront.h"

#include <stdlib.h>

/**********************************************************
* Function: tmNode_create()
*----------------------------------------------------------
* Create a new tmNode structure and returns a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new node
*
* @return: Pointer to a new tmNode structure
**********************************************************/
tmNode *tmNode_create(tmMesh *mesh, tmDouble xy[2])
{
  tmNode *node = (tmNode*) calloc( 1, sizeof(tmNode) );
  check_mem(node);

  /*-------------------------------------------------------
  | Add node to stack of mesh
  -------------------------------------------------------*/
  node->mesh      = mesh;
  node->qtree_pos = NULL;

  node->xy[0]     = xy[0];
  node->xy[1]     = xy[1];
  node->index     = mesh->no_nodes;
  node->is_active = TRUE;

  /*-------------------------------------------------------
  | Init node properties
  -------------------------------------------------------*/
  node->on_bdry     = FALSE;
  node->on_front    = FALSE;

  /*-------------------------------------------------------
  | Init node lists
  -------------------------------------------------------*/
  node->bdry_edges   = tmList_create();
  node->n_bdry_edges = 0;

  node->front_edges   = tmList_create();
  node->n_front_edges = 0;

  node->mesh_edges    = tmList_create();
  node->n_mesh_edges  = 0;

  node->tris   = tmList_create();
  node->n_tris = 0;

  /*-------------------------------------------------------
  | Init buffer variables ( e.g. for sorting )
  -------------------------------------------------------*/
  node->dblBuf = 0.0;
  node->indBuf = 0;

  /*-------------------------------------------------------
  | Size function properties if node is on boundary
  -------------------------------------------------------*/
  node->rho = 0.0;
  node->k   = 0.0;

  /*-------------------------------------------------------
  | Add node to qtree
  -------------------------------------------------------*/
  node->stack_pos = tmMesh_addNode(mesh, node);

  return node;
error:
  return NULL;
  
} /* tmNode_create() */

/**********************************************************
* Function: tmNode_destroy()
*----------------------------------------------------------
* Destroys a tmNode structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode to destroy
**********************************************************/ 
void tmNode_destroy(tmNode *node)
{
  /*-------------------------------------------------------
  | Remove node from the mesh structure 
  -------------------------------------------------------*/
  tmMesh_remNode(node->mesh, node);

  /*-------------------------------------------------------
  | Free all list structures
  -------------------------------------------------------*/
  tmList_destroy(node->front_edges);
  tmList_destroy(node->bdry_edges);
  tmList_destroy(node->mesh_edges);
  tmList_destroy(node->tris);

  /*-------------------------------------------------------
  | Finally free node structure memory
  -------------------------------------------------------*/
  free(node);

} /* tmNode_destroy() */

/**********************************************************
* Function: tmNode_getBdryEdgeIn()
*----------------------------------------------------------
* Returns a list of ingoing boundary edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: tmList of tmEdges
**********************************************************/
tmList *tmNode_getBdryEdgeIn(tmNode *node)
{
  tmListNode *cur;
  tmEdge   *cur_edge;
  tmList     *found_edges = tmList_create();

  for ( cur = node->bdry_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n2 == node )
      tmList_push( found_edges, cur_edge );
  }

  return found_edges;

} /* tmNode_getBdryEdgeIn() */

/**********************************************************
* Function: tmNode_getBdryEdgeOut()
*----------------------------------------------------------
* Returns a list of outgoing boundary edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: tmList of tmEdges
**********************************************************/
tmList *tmNode_getBdryEdgeOut(tmNode *node)
{
  tmListNode *cur;
  tmEdge   *cur_edge;
  tmList     *found_edges = tmList_create();

  for ( cur = node->bdry_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n1 == node )
      tmList_push( found_edges, cur_edge );
  }

  return found_edges;

} /* tmNode_getBdryEdgeOut() */

/**********************************************************
* Function: tmNode_getFrontEdgeIn()
*----------------------------------------------------------
* Returns a list of ingoing advancing front edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: tmList of tmEdges
**********************************************************/
tmList *tmNode_getFrontEdgeIn(tmNode *node)
{
  tmListNode *cur;
  tmEdge   *cur_edge;
  tmList     *found_edges = tmList_create();

  for ( cur = node->front_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n2 == node )
      tmList_push( found_edges, cur_edge );
  }

  return found_edges;

} /* tmNode_getFrontEdgeIn() */

/**********************************************************
* Function: tmNode_getFrontEdgeOut()
*----------------------------------------------------------
* Returns a list of outgoing advancing front edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: tmList of tmEdges
**********************************************************/
tmList *tmNode_getFrontEdgeOut(tmNode *node)
{
  tmListNode *cur;
  tmEdge   *cur_edge;
  tmList     *found_edges = tmList_create();

  for ( cur = node->front_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n1 == node )
      tmList_push( found_edges, cur_edge );
  }

  return found_edges;

} /* tmNode_getFrontEdgeOut() */


/**********************************************************
* Function: tmNode_compareDblBuf()
*----------------------------------------------------------
* Compare the tmDouble buffers of two nodes
*
* -> Returns an integer less than zero, if dblBuf of n1 is 
*    less than dblBuf of n2
*
* -> Returns zero, if dblBuf of n1 equals dblBuf of n2
*    |-> IMPORTANT FOR FINDING NEIGHBOURING NODES
*    |   ( initial node is ignored through this )
*
*  -> Returns an integer greater than zero if dblBuf of 
*     n1 is greater than dblBuf of n2
*----------------------------------------------------------
* @param *n1, *n2: pointer to tmNodes to compare
**********************************************************/
int tmNode_compareDblBuf(tmNode *n1, tmNode *n2)
{
  if (n1->dblBuf > n2->dblBuf)
    return 1;

  if EQ(n1->dblBuf, n2->dblBuf)
    return 0;

  return -1;

} /* tmNode_compareDblBuf() */

/**********************************************************
* Function: tmNode_getNbrsFromSizeFun
*----------------------------------------------------------
* Function to estimate neighbouring nodes of a given node
* within a range that is estimated from a sizefunction
*
* ! node is last entry in list *inCirc !
*
*----------------------------------------------------------
* 
**********************************************************/
tmList *tmNode_getNbrsFromSizeFun(tmNode *node)
{
  tmSizeFun sizeFun = node->mesh->sizeFun;
  tmDouble *xy      = node->xy;
  tmMesh   *mesh    = node->mesh;
  tmDouble  r       = TM_NODE_NBR_DIST_FAC*sizeFun(mesh,xy);

  tmList *inCirc = tmQtree_getObjCirc(node->mesh->nodes_qtree,
                                    node->xy,
                                    r);

  /*-------------------------------------------------------
  | inCirc can be NULL, if node is outside of the domain   
  -------------------------------------------------------*/
  if (inCirc == NULL)
    return NULL;

  /*-------------------------------------------------------
  | Sort neighbors by distance to node
  | -> Distance to node is already buffered in dblBuf 
  |    of node by tmQtree_getObjCirc()
  -------------------------------------------------------*/
  tmList *inCirc_sorted;
  inCirc_sorted = tmList_merge_sort(inCirc,
                        (tmList_compare) tmNode_compareDblBuf);
  check( inCirc_sorted != NULL,
      "tmList sort in tmNode_getNbrsFromSizeFun() failed.");

  if (inCirc != inCirc_sorted)
    tmList_destroy(inCirc);

  return inCirc_sorted; 


error:
  return NULL;

} /* tmNode_getNbrsFromSizeFun() */

/**********************************************************
* Function: tmNode_getFrontNbrs
*----------------------------------------------------------
* Returns a list of tmNodes that are connected to a
* given tmNode through advancing front edges
*
*----------------------------------------------------------
* 
**********************************************************/
tmList *tmNode_getFrontNbrs(tmNode *node)
{
  tmListNode *cur  = NULL;
  tmList     *nbrs = tmList_create();

  for (cur = node->front_edges->first;
       cur != NULL; cur = cur->next)
  {
    tmEdge *e  = (tmEdge*)cur->value;

    if ( node == e->n1 )
      tmList_push( nbrs, e->n2 );
    else if ( node == e->n2 )
      tmList_push( nbrs, e->n1 );
    else
      log_err("Invalid edge found in tmNode_getFrontNbrs().");
  }

  return nbrs;
error:
  return NULL;

} /* tmNode_getFrontNbrs() */

/**********************************************************
* Function: tmNode_getAdjFrontEdge()
*----------------------------------------------------------
* Checks for a node n wether it is connected to a 
* node m through an advancing front edge.
* If yes, the edge is returned - otherwise a NULL pointer
* is returned
*
*----------------------------------------------------------
* @param n: node whose advancing front edges are checked
* @param m: node which is search for on edges other ends
* 
**********************************************************/
tmEdge *tmNode_getAdjFrontEdge(tmNode *n, tmNode *m)
{
  tmListNode *cur  = NULL;

  for (cur = n->front_edges->first;
       cur != NULL; cur = cur->next)
  {
    tmEdge *e  = (tmEdge*)cur->value;

    if ( m == e->n1 || m == e->n2 )
      return e;
  }

  return NULL;

} /* tmNode_getAdjFrontEdge() */

/**********************************************************
* Function: tmNode_isValid()
*----------------------------------------------------------
* Checks, if a node is valid for the mesh genereation 
* process. The node may not be located too close 
* to preceeding edges in its vicinity in order to be valid
*
*----------------------------------------------------------
* @param node: node to be checked
* 
**********************************************************/
tmBool tmNode_isValid(tmNode *node)
{
  tmDouble fac = TM_NODE_EDGE_DIST_FAC;

  tmMesh   *mesh    = node->mesh;
  tmSizeFun sizeFun = mesh->sizeFun;
  tmDouble  r       = sizeFun(mesh, node->xy);

  tmDouble  dist    = r * fac;
  tmDouble  dist2   = dist * dist;

  tmList     *inCirc;
  tmListNode *cur, *cur_bdry;
  tmQtree  *cur_qtree;

  /*-------------------------------------------------------
  | 0) Check if node is within the domain
  -------------------------------------------------------*/
  if ( tmMesh_objInside(mesh, node->xy) == FALSE )
  {
#if (TM_DEBUG > 1)
    tmPrint(" -> REJECTED: NEW-NODE OUTSIDE OF DOMAIN");
#endif
    return FALSE;
  }

  /*-------------------------------------------------------
  | 1) Get boundary edges in vicinity of node
  |    Check that normal distance of node to all edges
  |    is larger than fac*d
  -------------------------------------------------------*/
  for (cur_bdry = mesh->bdry_stack->first;
       cur_bdry != NULL; cur_bdry = cur_bdry->next)
  {
    cur_qtree = ((tmBdry*)cur_bdry->value)->edges_qtree;
    inCirc    = tmQtree_getObjCirc(cur_qtree, node->xy, r);

    if ( inCirc != NULL )
    {
      for (cur = inCirc->first; cur != NULL; cur = cur->next)
      {
        tmNode *n1 = ((tmEdge*)cur->value)->n1;
        tmNode *n2 = ((tmEdge*)cur->value)->n2;

        if ( EDGE_NODE_DIST2(n1->xy, n2->xy, node->xy) < dist2 )
        {
#if (TM_DEBUG > 1)
          tmPrint(" -> REJECTED: NODE TOO CLOSE TO BOUNDARY");
#endif
          tmList_destroy(inCirc);
          return FALSE;
        }
      }
    }

    if ( inCirc != NULL )
      tmList_destroy(inCirc);
  }

  /*-------------------------------------------------------
  | 2) Get front edges in vicinity of node
  |    Check that normal distance of node to all edges
  |    is larger than fac*d
  -------------------------------------------------------*/
  cur_qtree = mesh->front->edges_qtree;
  inCirc    = tmQtree_getObjCirc(cur_qtree, node->xy, r);

  if ( inCirc != NULL)
  {
    for (cur = inCirc->first; cur != NULL; cur = cur->next)
    {
      tmNode *n1 = ((tmEdge*)cur->value)->n1;
      tmNode *n2 = ((tmEdge*)cur->value)->n2;

      if ( EDGE_NODE_DIST2(n1->xy, n2->xy, node->xy) < dist2 )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: NODE TOO CLOSE TO FRONT");
#endif
        tmList_destroy(inCirc);
        return FALSE;
      }
    }
  }

  if ( inCirc != NULL )
    tmList_destroy(inCirc);

  /*-------------------------------------------------------
  | 4) Get nodes in vicinity of node
  |    Check that normal distance of node to all nodes
  |    is larger than fac*d
  -------------------------------------------------------*/
  cur_qtree = mesh->nodes_qtree;
  inCirc    = tmQtree_getObjCirc(cur_qtree, node->xy, r);

  if ( inCirc != NULL )
  {
    for (cur = inCirc->first; cur != NULL; cur = cur->next)
    {
      tmNode *n = (tmNode*)cur->value;

      if (n == node)
        continue;

      tmDouble nn_dx = n->xy[0] - node->xy[0];
      tmDouble nn_dy = n->xy[1] - node->xy[1];
      tmDouble nn_dist2 = nn_dx*nn_dx + nn_dy*nn_dy;

      if ( nn_dist2 < dist2 )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: NODE TOO CLOSE TO NODE");
#endif
        tmList_destroy(inCirc);
        return FALSE;
      }

    }
  }

  if ( inCirc != NULL )
    tmList_destroy(inCirc);


  return TRUE;

} /* tmNode_isValid() */


