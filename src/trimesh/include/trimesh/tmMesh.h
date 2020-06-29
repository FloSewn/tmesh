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
  tmDouble xy_min[2];
  tmDouble xy_max[2];

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  tmNode  *nodes_head;
  List    *nodes_stack;
  int      no_nodes;
  tmQtree *nodes_qtree;

  /*-------------------------------------------------------
  | Mesh boundaries 
  -------------------------------------------------------*/
  int    no_bdrys;
  List  *bdry_stack;

  /*-------------------------------------------------------
  | Mesh advancing front edges 
  -------------------------------------------------------*/
  tmFront *front;

  /*-------------------------------------------------------
  | Mesh size function
  -------------------------------------------------------*/
  tmSizeFun sizeFun;

  /*-------------------------------------------------------
  | Mesh edges
  -------------------------------------------------------*/
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
  int      no_tris_delaunay;

} tmMesh;


/**********************************************************
* Function: tmMesh_create()
*----------------------------------------------------------
* Create a new tmMesh structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmMesh structure
**********************************************************/
tmMesh *tmMesh_create(tmDouble  xy_min[2], 
                      tmDouble  xy_max[2],
                      int       qtree_max_obj,
                      tmSizeFun sizeFun);

/**********************************************************
* Function: tmMesh_destroy()
*----------------------------------------------------------
* Destroys a tmMesh structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmMesh to destroy
*
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
* Function to add an edge to a tmMesh structure
*----------------------------------------------------------
* 
**********************************************************/
tmEdge *tmMesh_addEdge(tmMesh *mesh, 
                       tmNode *n1, tmNode *n2,
                       tmTri  *t1, tmTri  *t2);

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
* Function to remove an edge from a tmMesh structure
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

/**********************************************************
* Function: tmMesh_addBdry()
*----------------------------------------------------------
* Function to add a boundary structure to a tmMesh
*----------------------------------------------------------
* @return: tmBdry pointer
**********************************************************/
tmBdry *tmMesh_addBdry(tmMesh *mesh,
                       tmBool  is_interior,
                       int     index);

/**********************************************************
* Function: tmMesh_remBdry()
*----------------------------------------------------------
* Function to remove a boundary structure from a tmMesh
*----------------------------------------------------------
*
**********************************************************/
void tmMesh_remBdry(tmMesh *mesh, tmBdry *bdry);

/**********************************************************
* Function: tmMesh_objInside()
*----------------------------------------------------------
* Function to check wether an object is contained within
* the mesh boundary
*----------------------------------------------------------
* @param mesh
* @param obj
* @param obj_type
**********************************************************/
tmBool tmMesh_objInside(tmMesh *mesh, void *obj, int obj_type);

/**********************************************************
* Function: tmMesh_printMesh()
*----------------------------------------------------------
* Fuction to print out the mesh data
*----------------------------------------------------------
* @param mesh: pointer to mesh structure
**********************************************************/
void tmMesh_printMesh(tmMesh *mesh);

/**********************************************************
* Function: tmMesh_adfMeshing()
*----------------------------------------------------------
* Function to perform the advancing front loop 
* until either no edges are available anymore or until 
* a final iteration is reached
*----------------------------------------------------------
* 
**********************************************************/
void tmMesh_ADFMeshing(tmMesh *mesh);

#endif
