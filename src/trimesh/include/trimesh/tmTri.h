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
  tmIndex index;

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

  /*-------------------------------------------------------
  | Triangle area
  -------------------------------------------------------*/
  tmDouble area;

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

#endif
