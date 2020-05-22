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

  /*-------------------------------------------------------
  | Index of this node and its state
  -------------------------------------------------------*/
  ListNode *stack_pos;
  tmBool    is_active;

  /*-------------------------------------------------------
  | Boundary properties if node is located on a boundary
  -------------------------------------------------------*/
  tmBool  on_bdry;
  tmNode *bdry_succ;
  tmNode *bdry_prev;

  /*-------------------------------------------------------
  | All adjacent edges to this node 
  -------------------------------------------------------*/
  List *edges;
  int   n_edges;

  /*-------------------------------------------------------
  | All adjacent triangles to this node 
  -------------------------------------------------------*/
  List *tris;
  int   n_tris;

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



#endif
