#ifndef TRIMESH_TMBDRY_H
#define TRIMESH_TMBDRY_H

#include "trimesh/tmTypedefs.h"

/**********************************************************
* tmBdry: Boundary structure
**********************************************************/
typedef struct tmBdry {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  tmMesh *mesh;

  /*-------------------------------------------------------
  | Properties of this boundary
  -------------------------------------------------------*/
  int    bdry_index;
  tmBool is_exterior;
  tmBool is_closed;
  int    no_edges;

  /*-------------------------------------------------------
  | Boundary nodes
  -------------------------------------------------------*/
  tmNode *nodes_start;
  tmNode *nodes_end;

  /*-------------------------------------------------------
  | Boundary edges
  -------------------------------------------------------*/
  tmEdge  *edges_start;
  tmEdge  *edges_end;

} tmBdry;


/**********************************************************
* Function: tmBdry_create()
*----------------------------------------------------------
* Create a new tmBdry structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmBdry structure
**********************************************************/
tmBdry *tmBdry_create(tmMesh *mesh);

/**********************************************************
* Function: tmBdry_destroy()
*----------------------------------------------------------
* Destroys a tmBdry structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmBdry to destroy
*
**********************************************************/
void tmBdry_destroy(tmBdry *bdry);

/**********************************************************
* Function: tmBdry_init()
*----------------------------------------------------------
* Inits a tmBdry structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmBdry structure
**********************************************************/
tmBdry *tmBdry_init(tmBdry *bdry, 
                    int     index, 
                    tmBool  is_exterior);

/**********************************************************
* Function: tmBdry_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmBdry structure
*----------------------------------------------------------
* @return: ListNode to tmEdge on the mesh's edge stack
**********************************************************/
void tmBdry_addEdge(tmBdry *bdry, 
                    tmNode *n1, 
                    tmNode *n2);

/**********************************************************
* Function: tmBdry_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmBdry structure
*----------------------------------------------------------
*
**********************************************************/
void tmBdry_remEdge(tmBdry *bdry, tmEdge *edge);


#endif
