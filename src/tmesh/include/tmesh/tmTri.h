#ifndef TMESH_TMTRI_H
#define TMESH_TMTRI_H

#include "tmesh/tmTypedefs.h"

/**********************************************************
* tmTri: Triangle structure
**********************************************************/
typedef struct tmTri {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  tmMesh *mesh;

  /*-------------------------------------------------------
  | Triangle index
  -------------------------------------------------------*/
  ListNode *stack_pos;
  ListNode *qtree_pos;
  tmQtree  *qtree;

  /*-------------------------------------------------------
  | Triangle nodes
  | Aligned in counter-clockwise orientation
  -------------------------------------------------------*/
  tmNode *n1;
  tmNode *n2;
  tmNode *n3;

  /*-------------------------------------------------------
  | Position of this triangle in its nodes triangle-lists
  | -> Every node has a list with its adjacent triangles,
  |    These references recover the position of this 
  |    triangle in the lists of its nodes
  -------------------------------------------------------*/
  ListNode *n1_pos;
  ListNode *n2_pos;
  ListNode *n3_pos;

  /*-------------------------------------------------------
  | Triangle neighbors
  | Aligned in counter-clockwise orientation
  | t1: located opposite of n1
  |     shares e1
  | t2: located opposite of n2
  |     shares e2
  | t3: located pposite of n3
  |     shares e3
  -------------------------------------------------------*/
  tmTri  *t1;
  tmTri  *t2;
  tmTri  *t3;
  
  /*-------------------------------------------------------
  | Triangle edges ( only edges of the tmMesh_edgeStack )
  | Aligned in counter-clockwise orientation
  | e1: located opposite of n1
  |     adjacent to t1
  | e2: located opposite of n2
  |     adjacent to t2
  | e3: located pposite of n3
  |     adjacent to t3
  -------------------------------------------------------*/
  tmEdge *e1;
  tmEdge *e2;
  tmEdge *e3;

  /*-------------------------------------------------------
  | Centroid coordinates
  -------------------------------------------------------*/
  tmDouble xy[2];
  tmIndex  index; /* Only used for the mesh-output       */       

  /*-------------------------------------------------------
  | Triangle properties
  -------------------------------------------------------*/
  tmDouble edgeLen[3];
  tmDouble area;
  tmDouble shapeFac;
  tmDouble angles[3];
  tmDouble minAngle;
  tmDouble maxAngle;
  tmDouble quality;
  
  /*-------------------------------------------------------
  | Circumcenter und circumradius
  -------------------------------------------------------*/
  tmDouble circ_xy[2];
  tmDouble circ_r;
  tmBool   is_delaunay;

  /*-------------------------------------------------------
  | Buffer variables ( e.g. for sorting )
  -------------------------------------------------------*/
  tmDouble dblBuf;
  tmIndex  indBuf;

} tmTri;



/**********************************************************
* Function: tmTri_create()
*----------------------------------------------------------
* Create a new tmTri structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new triangle
* @param n1,n2,n3: nodes defining the triangle
*
* @return: Pointer to a new tmTri structure
**********************************************************/
tmTri *tmTri_create(tmMesh *mesh,
                    tmNode *n1, tmNode *n2, tmNode *n3);


/**********************************************************
* Function: tmTri_destroy()
*----------------------------------------------------------
* Destroys a tmTri structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmTri to destroy
**********************************************************/
void tmTri_destroy(tmTri *tri);

/**********************************************************
* Function: tmTri_findNbrTriFromEdge()
*----------------------------------------------------------
* Searches for a neighboring triangle, which is adjacent
* to an edge defined by two vertices (n1, n2)
* The arrangement of n1 and n2 plays no role.
*----------------------------------------------------------
* @param n1,n2: nodes defining an edge, for which a
*               triangle must be found
* @param tri:   triangle for which a neighbor will be found
*               if one has been found, it will be included
*               to its neighbors
**********************************************************/
tmTri *tmTri_findTriNeighbor(tmNode *n1, 
                             tmNode *n2,
                             tmTri  *tri);

/**********************************************************
* Function: tmTri_isValid()
*----------------------------------------------------------
* Function to check wether a provided triangle is valid
*----------------------------------------------------------
* @param *tri: pointer to a tmTri 
**********************************************************/
tmBool tmTri_isValid(tmTri *tri);

/**********************************************************
* Function: tmTri_edgeIntersect()
*----------------------------------------------------------
* Function to check wether a provided triangles intersects
* with an edge, defined by its vertices e1, e2
*----------------------------------------------------------
* @param *t1: pointer to first tmTri 
* @param *t2: pointer to second tmTri 
**********************************************************/
tmBool tmTri_edgeIntersect(tmTri *t, tmNode *e1, tmNode *e2);

/**********************************************************
* Function: tmTri_triIntersectn()
*----------------------------------------------------------
* Function to check wether two provided triangles intersect
*----------------------------------------------------------
* @param *t1: pointer to first tmTri 
* @param *t2: pointer to second tmTri 
**********************************************************/
tmBool tmTri_triIntersect(tmTri *t1, tmTri *t2);

/**********************************************************
* Function: tmTri_nodeIntersect()
*----------------------------------------------------------
* Function to check wether a provided triangles intersects
* with a node
*----------------------------------------------------------
* @param *t: pointer to tmTri 
* @param *n: pointer to tmNode
**********************************************************/
tmBool tmTri_nodeIntersect(tmTri *t, tmNode *n);

#endif
