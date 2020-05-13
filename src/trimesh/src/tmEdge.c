#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmTri.h"


/**********************************************************
* Function: tmEdge_create()
*----------------------------------------------------------
* Create a new tmEdge structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new edge
* @param n1,n2: Start and ending nodes of this edge
*
* @return: Pointer to a new tmEdge structure
**********************************************************/
tmEdge *tmEdge_create(tmMesh *mesh, tmNode *n1, tmNode *n2)
{
  tmEdge *edge = (tmEdge*) calloc( 1, sizeof(tmEdge) );
  check_mem(edge);

  /*-------------------------------------------------------
  | Init nodes 
  -------------------------------------------------------*/
  edge->n1 = n1;
  edge->n2 = n2;

  /*-------------------------------------------------------
  | Add edge to stack of mesh
  -------------------------------------------------------*/
  edge->mesh  = mesh;

  /*-------------------------------------------------------
  | Init edge properties
  -------------------------------------------------------*/
  edge->is_active = FALSE;
  edge->is_on_front = FALSE;

  edge->xy[0] = 0.0;
  edge->xy[1] = 0.0;
  edge->len = 0.0;
  edge->dxy_t[0] = 0.0;
  edge->dxy_t[1] = 0.0;
  edge->dxy_n[0] = 0.0;
  edge->dxy_n[1] = 0.0;

  /*-------------------------------------------------------
  | Init triangles 
  -------------------------------------------------------*/
  edge->t1 = NULL;
  edge->t2 = NULL;

  /*-------------------------------------------------------
  | Add edge to qtree
  -------------------------------------------------------*/
  edge->index = tmMesh_addEdge(mesh, edge);
  
  return edge;
error:
  return NULL;


} /* tmEdge_create() */

/**********************************************************
* Function: tmEdge_destroy()
*----------------------------------------------------------
* Destroys a tmEdge structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmEdge to destroy
**********************************************************/
void tmEdge_destroy(tmEdge *edge)
{
  /*-------------------------------------------------------
  | Finally free edge structure memory
  -------------------------------------------------------*/
  free(edge);


} /* tmEdge_destroy() */
