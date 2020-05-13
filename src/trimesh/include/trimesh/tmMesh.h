#ifndef TRIMESH_TMMESH_H
#define TRIMESH_TMMESH_H

#include "trimesh/tmTypedefs.h"

/**********************************************************
* tmMesh: Triangular mesh structure
**********************************************************/
typedef struct tmMesh {

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  tmNode  *nodes_head;
  List    *nodes_stack;
  int      nodes_index;
  tmQtree *nodes_qtree;

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  tmEdge  *edges_head;
  List    *edges_stack;
  int      edges_index;
  tmQtree *edges_qtree;

  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  tmTri   *tris_head;
  List    *tris_stack;
  int      tris_index;
  tmQtree *tris_qtree;

} tmMesh;


/**********************************************************
* Function: tmMesh_create()
*----------------------------------------------------------
* Create a new tmMesh structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmMesh structure
**********************************************************/
tmMesh *tmMesh_create(tmDouble xy_min[2], tmDouble xy_max[2]);

/**********************************************************
* Function: tmMesh_destroy()
*----------------------------------------------------------
* Destroys a tmMesh structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmMesh to destroy
*
* @return: Returns TRUE if method works
**********************************************************/
void tmMesh_destroy(tmMesh *mesh);

/**********************************************************
* Function: tmMesh_addNode()
*----------------------------------------------------------
* Function to add a tmNode to a tmMesh
*----------------------------------------------------------
* @return: tmNode index on the mesh's node stack
**********************************************************/
tmIndex tmMesh_addNode(tmMesh *mesh, tmNode *node);

/**********************************************************
* Function: tmMesh_addEdge()
*----------------------------------------------------------
* Function to add a tmEdge to a tmMesh
*----------------------------------------------------------
* @return: tmEdge index on the mesh's edge stack
**********************************************************/
tmIndex tmMesh_addEdge(tmMesh *mesh, tmEdge *edge);

/**********************************************************
* Function: tmMesh_addTri()
*----------------------------------------------------------
* Function to add a tmTri to a tmMesh
*----------------------------------------------------------
* @return: tmTri index on the mesh's tri stack
**********************************************************/
tmIndex tmMesh_addTri(tmMesh *mesh, tmTri *tri);

/**********************************************************
* Function: tmMesh_remNode()
*----------------------------------------------------------
* Function to remove a tmNode from a tmMesh
*----------------------------------------------------------
* @return: tmNode index on the mesh's node stack
*          returns -1 if node was not found
**********************************************************/
tmIndex tmMesh_remNode(tmMesh *mesh, tmNode *node);

#endif
