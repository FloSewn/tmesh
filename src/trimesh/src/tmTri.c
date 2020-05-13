#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmTri.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"

/**********************************************************
* Function: tmTri_calcCentroid()
*----------------------------------------------------------
* Computes the centroid of a triangle
*----------------------------------------------------------
* @param tri: triangle structure
**********************************************************/
static void tmTri_calcCentroid(tmTri *tri);

/**********************************************************
* Function: tmTri_calcArea()
*----------------------------------------------------------
* Computes the area of a triangle
*----------------------------------------------------------
* @param tri: triangle structure
**********************************************************/
static void tmTri_calcArea(tmTri *tri);




/**********************************************************
* Function: tmTri_create()
*----------------------------------------------------------
* Create a new tmTri structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new triangle
* @param n1,n2,n3: nodes defining the triangle
*
* @return: Pointer to a new tmTri structure
**********************************************************/
tmTri *tmTri_create(tmMesh *mesh, 
                    tmNode *n1, tmNode *n2, tmNode *n3)
{
  tmTri *tri = (tmTri*) calloc( 1, sizeof(tmTri) );
  check_mem(tri);

  /*-------------------------------------------------------
  | Add tri to stack of mesh
  -------------------------------------------------------*/
  tri->mesh = mesh;

  /*-------------------------------------------------------
  | Init tri nodes
  -------------------------------------------------------*/
  tri->n1 = n1;
  tri->n2 = n2;
  tri->n3 = n3;

  /*-------------------------------------------------------
  | Init tri properties
  -------------------------------------------------------*/
  tri->xy[0] = 0.0;
  tri->xy[1] = 0.0;
  tri->area  = 0.0;

  /*-------------------------------------------------------
  | Init tri neighbors
  -------------------------------------------------------*/
  tri->t1 = NULL;
  tri->t2 = NULL;
  tri->t3 = NULL;

  /*-------------------------------------------------------
  | Add tri to qtree
  -------------------------------------------------------*/
  tri->index = tmMesh_addTri(mesh, tri);

  return tri;
error:
  return NULL;

} /* tmTri_create() */

/**********************************************************
* Function: tmTri_destroy()
*----------------------------------------------------------
* Destroys a tmTri structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmTri to destroy
**********************************************************/
void tmTri_destroy(tmTri *tri)
{
  /*-------------------------------------------------------
  | Finally free tri structure memory
  -------------------------------------------------------*/
  free(tri);

} /* tmTri_destroy() */
