#ifndef TRIMESH_TMNODE_H
#define TRIMESH_TMNODE_H

#include "trimesh/tmTypedefs.h"


/**********************************************************
* tmNode: Node re
**********************************************************/
typedef struct tmNode {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  tmMesh *mesh;

  /*-------------------------------------------------------
  | Node coordinates 
  -------------------------------------------------------*/
  tmDouble xy[2];
  tmIndex  index; /* Only used for the mesh-output       */       

  /*-------------------------------------------------------
  | Properties of this node and its state
  -------------------------------------------------------*/
  ListNode *stack_pos;
  ListNode *qtree_pos;
  tmQtree  *qtree;

  /*-------------------------------------------------------
  | Boundary properties if node is located on a boundary
  -------------------------------------------------------*/
  tmBool  on_bdry;
  List   *bdry_edges;
  int     n_bdry_edges;

  /*-------------------------------------------------------
  | All adjacent front edges to this node 
  -------------------------------------------------------*/
  tmBool  on_front;
  List   *front_edges;
  int     n_front_edges;

  /*-------------------------------------------------------
  | All adjacent triangles to this node 
  -------------------------------------------------------*/
  List *tris;
  int   n_tris;

  /*-------------------------------------------------------
  | Buffer variables ( e.g. for sorting )
  -------------------------------------------------------*/
  tmDouble dblBuf;
  tmIndex  indBuf;

} tmNode;


/**********************************************************
* Function: tmNode_create()
*----------------------------------------------------------
* Create a new tmNode structure and returns a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new node
* @param xy:   Node coordinates
*
* @return: Pointer to a new tmNode structure
**********************************************************/
tmNode *tmNode_create(tmMesh *mesh, tmDouble xy[2]);

/**********************************************************
* Function: tmNode_destroy()
*----------------------------------------------------------
* Destroys a tmNode structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode to destroy
*
* @return: Returns TRUE if method works
**********************************************************/
void tmNode_destroy(tmNode *node);

/**********************************************************
* Function: tmNode_getBdryEdgeIn()
*----------------------------------------------------------
* Returns a list of ingoing boundary edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: List of tmEdges
**********************************************************/
List *tmNode_getBdryEdgeIn(tmNode *node);

/**********************************************************
* Function: tmNode_getBdryEdgeOut()
*----------------------------------------------------------
* Returns a list of outgoing boundary edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: List of tmEdges
**********************************************************/
List *tmNode_getBdryEdgeOut(tmNode *node);

/**********************************************************
* Function: tmNode_getFrontEdgeIn()
*----------------------------------------------------------
* Returns a list of ingoing advancing front edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: List of tmEdges
**********************************************************/
List *tmNode_getFrontEdgeIn(tmNode *node);

/**********************************************************
* Function: tmNode_getFrontEdgeOut()
*----------------------------------------------------------
* Returns a list of outgoing advancing front edges
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode 
*
* @return: List of tmEdges
**********************************************************/
List *tmNode_getFrontEdgeOut(tmNode *node);

#endif
