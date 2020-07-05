#ifndef TMESH_QTREE_H
#define TMESH_QTREE_H

#include "tmesh/tmTypedefs.h"

/**********************************************************
* tmQtree: Quadtree structure
**********************************************************/
typedef struct tmQtree {

  /*-------------------------------------------------------
  | Parent mesh properties
  -------------------------------------------------------*/
  tmMesh *mesh;

  /*-------------------------------------------------------
  | Qtree layer starts at 0
  -------------------------------------------------------*/
  tmIndex layer;
  tmIndex max_layer;

  /*-------------------------------------------------------
  | Maximum number of objects the Qtree can contain
  -------------------------------------------------------*/
  int max_obj;
  int obj_type;

  /*-------------------------------------------------------
  | Bounding box of this Qtree
  -------------------------------------------------------*/
  tmDouble xy_min[2];
  tmDouble xy_max[2];

  /*-------------------------------------------------------
  | Centroid of this Qtree
  -------------------------------------------------------*/
  tmDouble xy[2];

  /*-------------------------------------------------------
  | Width / height of this Qtree
  -------------------------------------------------------*/
  tmDouble dxy[2];

  /*-------------------------------------------------------
  | Objects contained in this Qtree
  -------------------------------------------------------*/
  int   n_obj_tot;
  int   n_obj;
  List *obj;

  /*-------------------------------------------------------
  | Parent Qtree and children Qtrees
  -------------------------------------------------------*/
  tmQtree *parent;

  tmQtree *child_NE;
  tmQtree *child_NW;
  tmQtree *child_SW;
  tmQtree *child_SE;

  /*-------------------------------------------------------
  | Flag if this Qtree is splitted
  -------------------------------------------------------*/
  tmBool is_splitted;

} tmQtree;

/**********************************************************
* Function: tmQtree_create()
*----------------------------------------------------------
* Create a new tmQtree structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new Qtree
* @param obj_type: object type of this Qtree
*
* @return: Pointer to a new tmQtree structure
**********************************************************/
tmQtree *tmQtree_create(tmMesh *mesh, int obj_type);

/**********************************************************
* Function: tmQtree_destroy()
*----------------------------------------------------------
* Destroys a tmQtree structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmQtree to destroy
**********************************************************/
void tmQtree_destroy(tmQtree *qtree);

/**********************************************************
* Function: tmQtree_init()
*----------------------------------------------------------
* Initialize a tmQtree structure 
*----------------------------------------------------------
* @param qtree: tmQtree structure to initialize
*
**********************************************************/
void tmQtree_init(tmQtree *qtree, 
                  tmQtree *parent,    tmIndex  layer, 
                  tmDouble xy_min[2], tmDouble xy_max[2]);

/**********************************************************
* Function: tmQtree_addObj()
*----------------------------------------------------------
* Try to add an object to the qtree 
*----------------------------------------------------------
* @param qtree: tmQtree structure
* @param obj:   object to add to the qtree
**********************************************************/
tmBool tmQtree_addObj(tmQtree *qtree, void *obj);

/**********************************************************
* Function: tmQtree_remObj()
*----------------------------------------------------------
* Try to remove an object from the qtree 
*----------------------------------------------------------
* @param qtree: tmQtree structure
* @param obj:   object to remove from the qtree
**********************************************************/
tmBool tmQtree_remObj(tmQtree *qtree, void *obj);

/**********************************************************
* Function: tmQtree_getObjNo()
*----------------------------------------------------------
* Return the number of objects contained in this qtree
*----------------------------------------------------------
* @param qtree: tmQtree structure
**********************************************************/
int tmQtree_getObjNo(tmQtree *qtree);

/**********************************************************
* Function: tmQtree_containsObj()
*----------------------------------------------------------
* Check if a given objects i contained within this qtree
*----------------------------------------------------------
* @param qtree: tmQtree structure
* @param obj_type: object type of this Qtree
**********************************************************/
tmBool tmQtree_containsObj(tmQtree *qtree,  
                           void    *obj, 
                           int      mode);

/**********************************************************
* Function: tmQtree_getObjBbox()
*----------------------------------------------------------
* Return a list of objects that are contained within
* a specified bounding box
*----------------------------------------------------------
* @param qtree: tmQtree structure to initialize
* @param xy_min, xy_max: bounding box
*
**********************************************************/
List *tmQtree_getObjBbox(tmQtree *qtree, 
                         tmDouble xy_min[2], 
                         tmDouble xy_max[2]);

/**********************************************************
* Function: tmQtree_getObjCirc()
*----------------------------------------------------------
* Return a list of objects that are contained within
* a specified circle 
*----------------------------------------------------------
* @param qtree: tmQtree structure to initialize
* @param xy: circle centroid
* @param r: circle radius
*
**********************************************************/
List *tmQtree_getObjCirc(tmQtree *qtree, 
                         tmDouble xy[2], 
                         tmDouble r);

/**********************************************************
* Function: tmQtree_printQtree()
*----------------------------------------------------------
* Function to print out the qtree data
*----------------------------------------------------------
* @param qtree: tmQtree structure to initialize
*
**********************************************************/
void tmQtree_printQtree(tmQtree *qtree);

#endif
