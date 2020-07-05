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
  tmIndex  bdry_marker;

  /*-------------------------------------------------------
  | Properties of this edge
  -------------------------------------------------------*/
  tmBool    is_on_front;
  tmBool    is_on_bdry;
  tmBool    is_on_mesh;
  tmBool    is_local_delaunay;

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
  | t1: left triangle
  | t2: right triangle
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
                      tmBdry *bdry,
                      int     edgeType);

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

/**********************************************************
* Function: tmEdge_createNode()
*----------------------------------------------------------
* Create new node perpendicular to an edge with a distance
* according to a size function
*----------------------------------------------------------
* @param edge: pointer to edge
* 
**********************************************************/
tmNode *tmEdge_createNode(tmEdge *edge);

/**********************************************************
* Function: tmEdge_isLocalDelaunay()
*----------------------------------------------------------
* Check if an edge is locally delaunay
* If yes, put edge on the non-delaunay edge stack of the
* mesh
*----------------------------------------------------------
* @param edge: pointer to edge
* 
**********************************************************/
void tmEdge_isDelaunay(tmEdge *edge);

/**********************************************************
* Function: tmEdge_flipEdge()
*----------------------------------------------------------
* Perform an edge flip, in order to get a new edge
* which satisfies the delaunay constraint.
* Definition:
*
*                    n2
*                   /^^
*                  / | \
*                 /  |  \
*            e11 /   |   \ e22
*               /    |    \
*              /     |     \
*             /      |      \
*         p2 v   t1  |  t2   \ p1
*            \       |       ^
*             \      |e     /
*              \     |     /
*               \    |    /
*            e12 \   |   / e21
*                 \  |  /
*                  \ | / 
*                   v|/
*                    n1
*               
*  e:   (n1, n2)      -> Flip to e*: (p1, p2)
*  t1:  (n1, n2, p2)  -> Flip to t1* : (n1, p1, p2)
*  t2:  (n1, p1, n2)  -> Flip to t2* : (n2, p2, p1)
*
*  e11: (n2, p2)  e12: (p2, n2)
*  e21: (n1, p1)  e22: (p1, n2)
*            
*----------------------------------------------------------
* @param edge: pointer to edge to flip
* @return : pointer to new edge
* 
**********************************************************/
tmEdge *tmEdge_flipEdge(tmEdge *e);

#endif
