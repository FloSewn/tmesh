/*
 * This header file is part of the tmesh library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef TMESH_TMQUAD_H
#define TMESH_TMQUAD_H

#include "tmesh/tmTypedefs.h"

/**********************************************************
* tmQuad: Quadrilateral structure
**********************************************************/
typedef struct tmQuad {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  tmMesh *mesh;

  /*-------------------------------------------------------
  | Quadrilateral index
  -------------------------------------------------------*/
  tmListNode *stack_pos;
  tmListNode *qtree_pos;
  tmQtree    *qtree;

  /*-------------------------------------------------------
  | Quadrilateral nodes
  | Aligned in counter-clockwise orientation
  -------------------------------------------------------*/
  tmNode *n1;
  tmNode *n2;
  tmNode *n3;
  tmNode *n4;

  /*-------------------------------------------------------
  | Owning triangles
  -------------------------------------------------------*/
  tmTri *t1;
  tmTri *t2;

} tmQuad;


#endif /* TMESH_TMQUAD_H */
