#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmTri.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmQtree.h"

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

  node->xy[0] = xy[0];
  node->xy[1] = xy[1];

  /*-------------------------------------------------------
  | Init node properties
  -------------------------------------------------------*/
  node->on_bdry     = FALSE;
  node->on_front    = FALSE;

  /*-------------------------------------------------------
  | Init node lists
  -------------------------------------------------------*/
  node->bdry_edges   = List_create();
  node->n_bdry_edges = 0;

  node->front_edges   = List_create();
  node->n_front_edges = 0;

  node->tris   = List_create();
  node->n_tris = 0;

  /*-------------------------------------------------------
  | Init buffer variables ( e.g. for sorting )
  -------------------------------------------------------*/
  node->dblBuf = 0.0;
  node->indBuf = 0;

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
  List_destroy(node->front_edges);
  List_destroy(node->bdry_edges);
  List_destroy(node->tris);

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
* @return: List of tmEdges
**********************************************************/
List *tmNode_getBdryEdgeIn(tmNode *node)
{
  ListNode *cur;
  tmEdge   *cur_edge;
  List     *found_edges = List_create();

  for ( cur = node->bdry_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n2 == node )
      List_push( found_edges, cur_edge );
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
* @return: List of tmEdges
**********************************************************/
List *tmNode_getBdryEdgeOut(tmNode *node)
{
  ListNode *cur;
  tmEdge   *cur_edge;
  List     *found_edges = List_create();

  for ( cur = node->bdry_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n1 == node )
      List_push( found_edges, cur_edge );
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
* @return: List of tmEdges
**********************************************************/
List *tmNode_getFrontEdgeIn(tmNode *node)
{
  ListNode *cur;
  tmEdge   *cur_edge;
  List     *found_edges = List_create();

  for ( cur = node->front_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n2 == node )
      List_push( found_edges, cur_edge );
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
* @return: List of tmEdges
**********************************************************/
List *tmNode_getFrontEdgeOut(tmNode *node)
{
  ListNode *cur;
  tmEdge   *cur_edge;
  List     *found_edges = List_create();

  for ( cur = node->front_edges->first; 
        cur != NULL; cur = cur->next )
  {
    cur_edge = (tmEdge*) cur->value;
    if ( cur_edge->n1 == node )
      List_push( found_edges, cur_edge );
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
List *tmNode_getNbrsFromSizeFun(tmNode    *node, 
                                tmSizeFun  sizeFun)
{
  tmDouble r = sizeFun( node->xy );

  List *inCirc = tmQtree_getObjCirc(node->mesh->nodes_qtree,
                                    node->xy,
                                    r);

  /*-------------------------------------------------------
  | Sort neighbors by distance to node
  | -> Distance to node is already buffered in dblBuf 
  |    of node by tmQtree_getObjCirc()
  -------------------------------------------------------*/
  int sorted = List_bubble_sort(inCirc, 
                        (List_compare) tmNode_compareDblBuf);
  check( sorted == 0,
      "List sort in tmNode_getNbrsFromSizeFun() failed.");

  /*-------------------------------------------------------
  | !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  | node is last entry in list *inCirc,
  | because compare function <tmNode_compareDblBuf> 
  | returns a 0 when the distance between objects is zero
  | -> DEPENDS ALSO ON SORT ALGORITHM
  | !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  -------------------------------------------------------*/
  //if ( inCirc->last != NULL )
  //  List_remove( inCirc, inCirc->last );

  return inCirc;

error:
  return NULL;

} /* tmNode_getNbrsFromSizeFun() */
