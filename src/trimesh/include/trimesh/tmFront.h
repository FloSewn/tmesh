#ifndef TRIMESH_TMFRONT_H
#define TRIMESH_TMFRONT_H

#include "trimesh/tmTypedefs.h"

/**********************************************************
* tmBdry: Advancing front structure
**********************************************************/
typedef struct tmFront {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  tmMesh *mesh;

  /*-------------------------------------------------------
  | Properties of the advancing front
  -------------------------------------------------------*/
  int   no_edges;

  /*-------------------------------------------------------
  | Advancing front edges
  -------------------------------------------------------*/
  tmEdge  *edges_head;
  List    *edges_stack;
  tmQtree *edges_qtree;

} tmFront;


/**********************************************************
* Function: tmFront_create()
*----------------------------------------------------------
* Create a new tmFront structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmFront structure
**********************************************************/
tmFront *tmFront_create(tmMesh *mesh);

/**********************************************************
* Function: tmFront_destroy()
*----------------------------------------------------------
* Destroys a tmFront structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmFront to destroy
*
**********************************************************/ 
void tmFront_destroy(tmFront *front);

/**********************************************************
* Function: tmFront_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmFront structure
*----------------------------------------------------------
* @return: ListNode to tmEdge on the mesh's edge stack
**********************************************************/
tmEdge *tmFront_addEdge(tmFront *front, 
                        tmNode  *n1, 
                        tmNode * n2);

/**********************************************************
* Function: tmFront_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmBdry structure
*----------------------------------------------------------
*
**********************************************************/
void tmFront_remEdge(tmFront *front, tmEdge *edge);


#endif
