#ifndef TRIMESH_TMTRI_H
#define TRIMESH_TMTRI_H

#include "trimesh/tmTypedefs.h"

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
  
  /*-------------------------------------------------------
  | Circumcenter und circumradius
  -------------------------------------------------------*/
  tmDouble circ_xy[2];
  tmDouble circ_r;

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
* Function: tmTri_isValid()
*----------------------------------------------------------
* Function to check wether a provided triangle is valid
*----------------------------------------------------------
* @param *tri: pointer to a tmTri 
**********************************************************/
tmBool tmTri_isValid(tmTri *tri);

#endif
