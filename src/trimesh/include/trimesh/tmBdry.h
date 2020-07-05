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
  int    no_edges;
  int    is_interior;
  int    index;

  tmDouble area;

  /*-------------------------------------------------------
  | Boundary edges
  -------------------------------------------------------*/
  tmEdge   *edges_head;
  List     *edges_stack;
  tmQtree  *edges_qtree;
  ListNode *mesh_pos;

} tmBdry;


/**********************************************************
* Function: tmBdry_create()
*----------------------------------------------------------
* Create a new tmBdry structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmBdry structure
**********************************************************/
tmBdry *tmBdry_create(tmMesh *mesh, 
                      tmBool  is_interior,
                      int     index);

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
* Function: tmBdry_edgeCreate()
*----------------------------------------------------------
* Function to create a new boundary edge 
*----------------------------------------------------------
* 
**********************************************************/
tmEdge *tmBdry_edgeCreate(tmBdry *bdry, 
                          tmNode *n1, 
                          tmNode *n2,
                          tmIndex marker);

/**********************************************************
* Function: tmBdry_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmBdry structure
* This edge is the new head of the boundary structure
*----------------------------------------------------------
* @return: ListNode to tmEdge on the mesh's edge stack
**********************************************************/
ListNode *tmBdry_addEdge(tmBdry *bdry, tmEdge *edge);

/**********************************************************
* Function: tmBdry_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmBdry structure
*----------------------------------------------------------
*
**********************************************************/
void tmBdry_remEdge(tmBdry *bdry, tmEdge *edge);

/**********************************************************
* Function: tmBdry_isLeft()
*----------------------------------------------------------
* Check if an object is left of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isLeft(tmBdry *bdry, void *obj, int obj_type);

/**********************************************************
* Function: tmBdry_isLeftOn()
*----------------------------------------------------------
* Check if an object is left of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isLeftOn(tmBdry *bdry, void *obj, int obj_type);

/**********************************************************
* Function: tmBdry_isRight()
*----------------------------------------------------------
* Check if an object is right of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isRight(tmBdry *bdry, void *obj, int obj_type);

/**********************************************************
* Function: tmBdry_isRightOn()
*----------------------------------------------------------
* Check if an object is right of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isRightOn(tmBdry *bdry, void *obj, int obj_type);

/**********************************************************
* Function: tmBdry_isInside()
*----------------------------------------------------------
* Check if an object with position xy is inside of a 
* boundary which is enclosed by at least three edges,
* using the Ray-Method.
* If the object is located on the boundary edges,
* it is treated as lying inside.
* 
* Check out this source:
* http://alienryderflex.com/polygon/
*----------------------------------------------------------
* @param *bdry: pointer to a tmBdry 
* @param  xy:   location to check for
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isInside(tmBdry *bdry, tmDouble xy[2]);

/**********************************************************
* Function: tmBdry_splitEdge()
*----------------------------------------------------------
* Split an edge by inserting a new node on its centroid
* and splitting it into two new edges
* This edge will be pointing from its node n1 to the new
* node and a new edge will be created, which points
* from the new node to n2.
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @param *edge: pointer to a tmEdge 
* @return: pointer to newly created first edge
**********************************************************/
tmEdge *tmBdry_splitEdge(tmBdry *bdry, tmEdge *edge);

/**********************************************************
* Function: tmBdry_refine()
*----------------------------------------------------------
* Refine the edges of a boundary structure according to
* a size function
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @return: 
**********************************************************/
void tmBdry_refine(tmBdry *bdry);

/**********************************************************
* Function: tmBdry_calcArea()
*----------------------------------------------------------
* Computes the area enclosed by the boundary and sets
* it in the boundaries properties
*
* Reference:
* https://www.wikihow.com/Calculate-the-Area-of-a-Polygon
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @return: 
**********************************************************/
void tmBdry_calcArea(tmBdry *bdry);

#endif
