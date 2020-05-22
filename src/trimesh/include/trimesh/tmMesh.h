#ifndef TRIMESH_TMMESH_H
#define TRIMESH_TMMESH_H

#include "trimesh/tmTypedefs.h"

/**********************************************************
* tmMesh: Triangular mesh structure
**********************************************************/
typedef struct tmMesh {

  /*-------------------------------------------------------
  | Mesh properties 
  -------------------------------------------------------*/
  int      qtree_max_obj;

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  tmNode  *nodes_head;
  List    *nodes_stack;
  int      no_nodes;
  tmQtree *nodes_qtree;

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  tmEdge  *edges_head;
  List    *edges_stack;
  int      no_edges;
  tmQtree *edges_qtree;

  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  tmTri   *tris_head;
  List    *tris_stack;
  int      no_tris;
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
tmMesh *tmMesh_create(tmDouble xy_min[2], 
                      tmDouble xy_max[2],
                      int      qtree_max_obj);

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
ListNode *tmMesh_addNode(tmMesh *mesh, tmNode *node);

/**********************************************************
* Function: tmMesh_addEdge()
*----------------------------------------------------------
* Function to add a tmEdge to a tmMesh
*----------------------------------------------------------
* @return: tmEdge index on the mesh's edge stack
**********************************************************/
ListNode *tmMesh_addEdge(tmMesh *mesh, tmEdge *edge);

/**********************************************************
* Function: tmMesh_addTri()
*----------------------------------------------------------
* Function to add a tmTri to a tmMesh
*----------------------------------------------------------
* @return: tmTri index on the mesh's tri stack
**********************************************************/
ListNode *tmMesh_addTri(tmMesh *mesh, tmTri *tri);

/**********************************************************
* Function: tmMesh_remNode()
*----------------------------------------------------------
* Function to remove a tmNode from a tmMesh
*----------------------------------------------------------
*          
**********************************************************/
void tmMesh_remNode(tmMesh *mesh, tmNode *node);

/**********************************************************
* Function: tmMesh_remEdge()
*----------------------------------------------------------
* Function to remove a tmEdge from a tmMesh
*----------------------------------------------------------
*
**********************************************************/
void tmMesh_remEdge(tmMesh *mesh, tmEdge *edge);

/**********************************************************
* Function: tmMesh_remTri()
*----------------------------------------------------------
* Function to remove a tmTri from a tmMesh
*----------------------------------------------------------
*
**********************************************************/
void tmMesh_remTri(tmMesh *mesh, tmTri *tri);

#endif
