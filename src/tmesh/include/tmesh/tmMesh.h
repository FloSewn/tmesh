#ifndef TMESH_TMMESH_H
#define TMESH_TMMESH_H

#include "tmesh/tmTypedefs.h"

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

  tmDouble areaBdry; /* Mesh domain area defined by bdry */
  tmDouble areaTris; /* Mesh domain area defined by tris */

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  tmNode  *nodes_head;
  tmList  *nodes_stack;
  int      no_nodes;
  tmQtree *nodes_qtree;

  /*-------------------------------------------------------
  | Mesh boundaries 
  -------------------------------------------------------*/
  int      no_bdrys;
  tmList  *bdry_stack;

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
  tmList  *edges_stack;
  int      no_edges;
  tmQtree *edges_qtree;

  /*-------------------------------------------------------
  | Stack to keep track of non-Delaunay edges
  -------------------------------------------------------*/
  tmList    *delaunay_stack;

  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  tmTri   *tris_head;
  tmList  *tris_stack;
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
tmListNode *tmMesh_addNode(tmMesh *mesh, tmNode *node);

/**********************************************************
* Function: tmMesh_edgeCreate()
*----------------------------------------------------------
* Function to create a new edge for a tmMesh structure
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
* @param n1, n2: start/ending node of edge
* @param t1, t2: triangle to the left / right of the edge
**********************************************************/
tmEdge *tmMesh_edgeCreate(tmMesh *mesh, 
                          tmNode *n1, tmNode *n2,
                          tmTri  *t1, tmTri  *t2);

/**********************************************************
* Function: tmMesh_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmMesh structure
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
* @param edge: edge to add 
**********************************************************/
tmListNode *tmMesh_addEdge(tmMesh *mesh, tmEdge *edge);

/**********************************************************
* Function: tmMesh_addTri()
*----------------------------------------------------------
* Function to add a tmTri to a tmMesh
*----------------------------------------------------------
* @return: tmTri index on the mesh's tri stack
**********************************************************/
tmListNode *tmMesh_addTri(tmMesh *mesh, tmTri *tri);

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
* Function to check wether an object at position xy 
* is contained within the mesh boundary
*----------------------------------------------------------
* @param mesh
* @param xy
**********************************************************/
tmBool tmMesh_objInside(tmMesh   *mesh, tmDouble xy[2]);

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

/**********************************************************
* Function: tmMesh_delaunayFlip()
*----------------------------------------------------------
* Function to perform the flip algorithm on a triangulated
* mesh, in order to obtain a triangulation that is delaunay
*----------------------------------------------------------
* 
**********************************************************/
void tmMesh_delaunayFlip(tmMesh *mesh);

/**********************************************************
* Function: tmMesh_setTriNeighbors()
*----------------------------------------------------------
* Sets for every triangle the connectivity to its neigbors
* This is done, when the advancing front meshing 
* has finished.
*----------------------------------------------------------
* @param mesh: the mesh structure
**********************************************************/
void tmMesh_setTriNeighbors(tmMesh *mesh);

/**********************************************************
* Function: tmMesh_calcArea()
*----------------------------------------------------------
* Function to compute the area enclosed by the entire
* mesh domain
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
**********************************************************/
void tmMesh_calcArea(tmMesh *mesh);

/**********************************************************
* Function: tmMesh_getTriFromCoords()
*----------------------------------------------------------
* Function returns a triangle which contains a given 
* point (x,y). If the point is located on the edge
* of two triangles, the first one that has been located
* on the tri-stack will be chosen.
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
* @param xy: point coordinates
* @return: triangle, in which xy is contained.
**********************************************************/
tmTri* tmMesh_getTriFromCoords(tmMesh *mesh, tmDouble xy[2]);

/**********************************************************
* Function: tmMesh_refineLocal()
*----------------------------------------------------------
* Add a new node at a specified location in the mesh
* and subsequently re-triangulate the mesh in this area.
*----------------------------------------------------------
* @param mesh: the mesh structure
* @param xy:   coordinates of the new node to insert
*
**********************************************************/
void tmMesh_refineLocally(tmMesh *mesh, tmDouble xy[2]);

#endif
