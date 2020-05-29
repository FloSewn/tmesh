#ifndef TRIMESH_TMBDRYEDGE_H
#define TRIMESH_TMBDRYEDGE_H

#include "trimesh/tmTypedefs.h"

/**********************************************************
* tmEdge: Edge structure
**********************************************************/
typedef struct tmEdge {

  /*-------------------------------------------------------
  | Parent structures
  -------------------------------------------------------*/
  tmMesh  *mesh;
  tmBdry  *bdry;
  tmFront *front;

  /*-------------------------------------------------------
  | Edge centroid coordinates 
  -------------------------------------------------------*/
  tmDouble xy[2];
  tmIndex  index; /* Only used for the mesh-output       */       

  /*-------------------------------------------------------
  | Properties of this edge
  -------------------------------------------------------*/
  tmBool    is_on_front;
  tmBool    is_on_bdry;

  ListNode *stack_pos;
  ListNode *qtree_pos;
  tmQtree  *qtree;

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
  | Reference of edge in nodes edge lists
  -------------------------------------------------------*/
  ListNode *n1_pos;
  ListNode *n2_pos;

  /*-------------------------------------------------------
  | Adjacent triangles to this edge
  | t_1: left triangle
  | t_2: right triangle
  -------------------------------------------------------*/
  tmTri  *t1;
  tmTri  *t2;

  /*-------------------------------------------------------
  | Buffer variables ( e.g. for sorting )
  -------------------------------------------------------*/
  tmDouble dblBuf;
  tmIndex  indBuf;

} tmEdge;

/**********************************************************
* Function: tmEdge_create()
*----------------------------------------------------------
* Create a new tmEdge structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh:  parent mesh of the new edge
* @param n1,n2: Start and ending nodes of this edge
* @param on_bdry: flag for boundary / front edge type
*
* @return: Pointer to a new tmEdge structure
**********************************************************/
tmEdge *tmEdge_create(tmMesh *mesh, 
                      tmNode *n1, 
                      tmNode *n2,
                      tmBdry *bdry);

/**********************************************************
* Function: tmEdge_destroy()
*----------------------------------------------------------
* Destroys a tmEdge structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmEdge to destroy
**********************************************************/
void tmEdge_destroy(tmEdge *edge);

/**********************************************************
* Function: tmEdge_isLeft()
*----------------------------------------------------------
* Check if an object is left of the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isLeft(tmEdge *edge, void *obj, int obj_type);

/**********************************************************
* Function: tmEdge_isLeftOn()
*----------------------------------------------------------
* Check if an object is left of or on the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isLeftOn(tmEdge *edge, void *obj, int obj_type);

/**********************************************************
* Function: tmEdge_isRight()
*----------------------------------------------------------
* Check if an object is right of the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isRight(tmEdge *edge, void *obj, int obj_type);

/**********************************************************
* Function: tmEdge_isRightOn()
*----------------------------------------------------------
* Check if an object is right of or on the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isRightOn(tmEdge *edge, void *obj, int obj_type);

/**********************************************************
* Function: tmEdge_compareLen()
*----------------------------------------------------------
* Compare two edges according to their lengths
*
* -> Returns an integer less than zero, if length of e1 is 
*    less than lenght of e2
*
* -> Returns zero, if length of e1 equals length of e2
*
*  -> Returns an integer greater than zero if length of 
*     e1 is greater than length of e2
*----------------------------------------------------------
* @param *e1, *e2: pointer to tmEdges to compare
**********************************************************/
int tmEdge_compareLen(tmEdge *e1, tmEdge *e2);

#endif
