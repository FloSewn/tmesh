#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmBdry.h"


/**********************************************************
* Function: tmBdry_create()
*----------------------------------------------------------
* Create a new tmBdry structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmBdry structure
**********************************************************/
tmBdry *tmBdry_create(tmMesh *mesh)
{
  tmBdry *bdry = (tmBdry*) calloc( 1, sizeof(tmBdry) );
  check_mem(bdry);

  /*-------------------------------------------------------
  | Properties of this boundary
  -------------------------------------------------------*/
  bdry->mesh        = mesh;

  bdry->bdry_index  = -1;
  bdry->is_exterior = FALSE;
  bdry->is_closed   = FALSE;
  bdry->no_edges    =  0;

  /*-------------------------------------------------------
  | Boundary nodes
  -------------------------------------------------------*/
  bdry->nodes_start = NULL;
  bdry->nodes_end   = NULL;

  /*-------------------------------------------------------
  | Boundary edges
  -------------------------------------------------------*/
  bdry->edges_start = NULL;
  bdry->edges_end   = NULL;

  return bdry;

error:
  return NULL;

} /* tmBdry_create() */

/**********************************************************
* Function: tmBdry_destroy()
*----------------------------------------------------------
* Destroys a tmBdry structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmBdry to destroy
*
**********************************************************/
void tmBdry_destroy(tmBdry *bdry)
{

  free(bdry);

} /* tmBdry_destroy() */

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
                    tmBool  is_exterior)
{

  bdry->bdry_index  = index;
  bdry->is_exterior = is_exterior;

} /* tmBdry_init() */

/**********************************************************
* Function: tmBdry_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmBdry structure
*----------------------------------------------------------
* @return: ListNode to tmEdge on the mesh's edge stack
**********************************************************/
void tmBdry_addEdge(tmBdry *bdry, 
                    tmNode *n1, 
                    tmNode *n2)
{

  tmEdge *edge = tmEdge_create( bdry->mesh, n1, n2 );
  tmEdge_init( edge, bdry );



} /* tmBdry_addEdge() */

/**********************************************************
* Function: tmBdry_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmBdry structure
*----------------------------------------------------------
*
**********************************************************/
void tmBdry_remEdge(tmBdry *bdry, tmEdge *edge)
{
} /* tmBdry_remEdge() */
