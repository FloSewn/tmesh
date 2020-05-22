#ifndef TRIMESH_TMEDGE_H
#define TRIMESH_TMEDGE_H

#include "trimesh/tmTypedefs.h"

/**********************************************************
* tmEdge: Edge structure
**********************************************************/
typedef struct tmEdge {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  tmMesh *mesh;

  /*-------------------------------------------------------
  | Edge centroid coordinates 
  -------------------------------------------------------*/
  tmDouble xy[2];

  /*-------------------------------------------------------
  | Index of this edge
  -------------------------------------------------------*/
  tmBool    is_active;
  tmBool    is_on_front;
  ListNode *stack_pos;

  /*-------------------------------------------------------
  | Edge length  
  -------------------------------------------------------*/
  tmDouble len;

  /*-------------------------------------------------------
  | Vector tangential to edge
  -------------------------------------------------------*/
  tmDouble dxy_t[2];

  /*-------------------------------------------------------
  | Vector normal to edge
  -------------------------------------------------------*/
  tmDouble dxy_n[2];

  /*-------------------------------------------------------
  | Edge start (n_1) and ending (n_2) nodes
  -------------------------------------------------------*/
  tmNode *n1;
  tmNode *n2;

  /*-------------------------------------------------------
  | Adjacent triangles to this edge
  | t_1: left triangle
  | t_2: right triangle
  -------------------------------------------------------*/
  tmTri  *t1;
  tmTri  *t2;

} tmEdge;

/**********************************************************
* Function: tmEdge_create()
*----------------------------------------------------------
* Create a new tmEdge structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh:  parent mesh of the new edge
* @param n1,n2: Start and ending nodes of this edge
*
* @return: Pointer to a new tmEdge structure
**********************************************************/
tmEdge *tmEdge_create(tmMesh *mesh, tmNode *n1, tmNode *n2);

/**********************************************************
* Function: tmEdge_destroy()
*----------------------------------------------------------
* Destroys a tmEdge structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmEdge to destroy
**********************************************************/
void tmEdge_destroy(tmEdge *edge);

#endif
