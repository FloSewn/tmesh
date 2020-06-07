#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmTri.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmQtree.h"

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
* Function: tmTri_calcCircumcenter()
*----------------------------------------------------------
* Computes the circumcenter and the circumradius 
* of a triangle
*----------------------------------------------------------
* @param tri: triangle structure
**********************************************************/
static void tmTri_calcCircumcenter(tmTri *tri);

/**********************************************************
* Function: tmTri_calcEdgeLen()
*----------------------------------------------------------
* Computes the edge lengths of a triangle
* Edge lengths are stored in the following structure:
* edgeLen[0] : n2->n3
* edgeLen[1] : n3->n1
* edgeLen[2] : n1->n2
*----------------------------------------------------------
* @param tri: triangle structure
**********************************************************/
static void tmTri_calcEdgeLen(tmTri *tri);

/**********************************************************
* Function: tmTri_calcShapeFac()
*----------------------------------------------------------
* Compute the geometrical shape factor of a triangle
* factor -> 1 for equilateral triangles
* factor -> 0 for bad triangles
*----------------------------------------------------------
* @param tri: triangle structure
**********************************************************/
static void tmTri_calcShapeFac(tmTri *tri);

/**********************************************************
* Function: tmTri_calcAngles()
*----------------------------------------------------------
* Computes the angles of a triangle
*----------------------------------------------------------
* @param tri: triangle structure
**********************************************************/
static void tmTri_calcAngles(tmTri *tri);


/**********************************************************
*
**********************************************************/
static void tmTri_calcCentroid(tmTri *tri)
{
  tmDouble *p = tri->n1->xy;
  tmDouble *q = tri->n2->xy;
  tmDouble *r = tri->n3->xy;

  tri->xy[0] = (p[0] + q[0] + r[0]) / 3.0;
  tri->xy[1] = (p[1] + q[1] + r[1]) / 3.0;


} /* tmTri_calcCentroid() */

/**********************************************************
*
**********************************************************/
static void tmTri_calcArea(tmTri *tri)
{
  tmDouble *p = tri->n1->xy;
  tmDouble *q = tri->n2->xy;
  tmDouble *r = tri->n3->xy;

  tmDouble area2 = (q[0] - p[0]) * (r[1] - p[1])
                 - (r[0] - p[0]) * (q[1] - p[1]);

  tri->area = 0.5 * area2;

} /* tmTri_calcArea() */

/**********************************************************
*
**********************************************************/
static void tmTri_calcCircumcenter(tmTri *tri)
{
  tmDouble *p = tri->n1->xy;
  tmDouble *q = tri->n2->xy;
  tmDouble *r = tri->n3->xy;

  tmDouble Bx = q[0] - p[0];
  tmDouble By = q[1] - p[1];
  tmDouble Cx = r[0] - p[0];
  tmDouble Cy = r[1] - p[1];

  tmDouble  D = 2.0 * ( Bx*Cy - By*Cx);

  tmDouble Ux = ( Cy*(Bx*Bx + By*By) 
                - By*(Cx*Cx + Cy*Cy) ) / D;

  tmDouble Uy = ( Bx*(Cx*Cx + Cy*Cy) 
                - Cx*(Bx*Bx + By*By) ) / D;

  tri->circ_xy[0] = Ux;
  tri->circ_xy[1] = Uy;
  tri->circ_r     = sqrt(Ux*Ux + Uy*Uy);

} /* tmTri_calcCircumcenter() */

/**********************************************************
*
**********************************************************/
static void tmTri_calcEdgeLen(tmTri *tri)
{
  tmDouble *n1 = tri->n1->xy;
  tmDouble *n2 = tri->n2->xy;
  tmDouble *n3 = tri->n3->xy;

  tmDouble e1_x = (n3[0]-n2[0]);
  tmDouble e1_y = (n3[1]-n2[1]);

  tmDouble e2_x = (n1[0]-n3[0]);
  tmDouble e2_y = (n1[1]-n3[1]);

  tmDouble e3_x = (n2[0]-n1[0]);
  tmDouble e3_y = (n2[1]-n1[1]);

  tri->edgeLen[0] = sqrt( e1_x * e1_x + e1_y * e1_y );
  tri->edgeLen[1] = sqrt( e2_x * e2_x + e2_y * e2_y );
  tri->edgeLen[2] = sqrt( e3_x * e3_x + e3_y * e3_y );

} /* tmTri_calcEdgeLen() */

/**********************************************************
* 
**********************************************************/
static void tmTri_calcShapeFac(tmTri *tri)
{
  const tmDouble normFac = 3.4641016151377544;
  const tmDouble sgnArea = normFac * tri->area;
  const tmDouble e1      = tri->edgeLen[0];
  const tmDouble e2      = tri->edgeLen[1];
  const tmDouble e3      = tri->edgeLen[2];
  const tmDouble sumEdge = e1*e1 + e2*e2 + e3*e3; 

  tri->shapeFac = sgnArea / ( sumEdge + SMALL );

} /* tmTri_calcShapeFac() */

/**********************************************************
* 
**********************************************************/
static void tmTri_calcAngles(tmTri *tri)
{
  tmDouble *p = tri->n1->xy;
  tmDouble *q = tri->n2->xy;
  tmDouble *r = tri->n3->xy;

  tmDouble l1 = tri->edgeLen[0];
  tmDouble l2 = tri->edgeLen[1];
  tmDouble l3 = tri->edgeLen[2];

  tmDouble a1 = ( (q[0]-p[0])*(r[0]-p[0])
                + (q[1]-p[1])*(r[1]-p[1]) ) / (l2*l3);

  tmDouble a2 = ( (p[0]-q[0])*(r[0]-q[0])
                + (p[1]-q[1])*(r[1]-q[1]) ) / (l1*l3);

  tmDouble a3 = ( (p[0]-r[0])*(q[0]-r[0])
                + (p[1]-r[1])*(q[1]-r[1]) ) / (l1*l2);

  a1 = acos(a1);
  a2 = acos(a2);
  a3 = acos(a3);

  tri->angles[0] = a1;
  tri->angles[1] = a2;
  tri->angles[2] = a3;

  tmDouble aMin, aMax;
  aMin = (aMin = a1 < a2 ? a1 : a2) < a3 ? aMin : a3; 
  aMax = (aMax = a1 > a2 ? a1 : a2) > a3 ? aMax : a3; 

  tri->minAngle = aMin;
  tri->maxAngle = aMax;

} /* tmTri_calcAngles() */



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
  tri->mesh      = mesh;
  tri->index     = mesh->no_tris;
  tri->qtree_pos = NULL;

  /*-------------------------------------------------------
  | Init tri nodes
  -------------------------------------------------------*/
  tri->n1 = n1;
  tri->n2 = n2;
  tri->n3 = n3;

  /*-------------------------------------------------------
  | Init tri properties
  -------------------------------------------------------*/
  tri->xy[0]      = 0.0;
  tri->xy[1]      = 0.0;
  tri->area       = 0.0;
  tri->shapeFac   = 0.0;
  tri->angles[0]  = 0.0;
  tri->angles[1]  = 0.0;
  tri->angles[2]  = 0.0;
  tri->minAngle   = 0.0;
  tri->maxAngle   = 0.0;
  tri->edgeLen[0] = 0.0;
  tri->edgeLen[1] = 0.0;
  tri->edgeLen[2] = 0.0;
  tri->circ_xy[0] = 0.0;
  tri->circ_xy[1] = 0.0;
  tri->circ_r     = 0.0;


  /*-------------------------------------------------------
  | Init tri neighbors
  -------------------------------------------------------*/
  tri->t1 = NULL;
  tri->t2 = NULL;
  tri->t3 = NULL;

  /*-------------------------------------------------------
  | Init buffer variables ( e.g. for sorting )
  -------------------------------------------------------*/
  tri->dblBuf = 0.0;
  tri->indBuf = 0;

  /*-------------------------------------------------------
  | 
  -------------------------------------------------------*/
  tmTri_calcCentroid(tri);
  tmTri_calcArea(tri);
  tmTri_calcCircumcenter(tri);
  tmTri_calcEdgeLen(tri);
  tmTri_calcShapeFac(tri);
  tmTri_calcAngles(tri);

  /*-------------------------------------------------------
  | Add tri to qtree
  -------------------------------------------------------*/
  tri->stack_pos = tmMesh_addTri(mesh, tri);

  return tri;
error:
  return NULL;

} /* tmTri_create() */

/**********************************************************
* Function: tmTri_destroy()
*----------------------------------------------------------
* Destroys a tmTri structure and frees all its memory.
*----------------------------------------------------------
* @param *tri: pointer to a tmTri to destroy
**********************************************************/
void tmTri_destroy(tmTri *tri)
{
  /*-------------------------------------------------------
  | Remove triangle from qtree 
  -------------------------------------------------------*/
  tmMesh_remTri(tri->mesh, tri);

  /*-------------------------------------------------------
  | Finally free tri structure memory
  -------------------------------------------------------*/
  free(tri);

} /* tmTri_destroy() */

/**********************************************************
* Function: tmTri_isValid()
*----------------------------------------------------------
* Function to check wether a provided triangle is valid
*----------------------------------------------------------
* @param *tri: pointer to a tmTri 
**********************************************************/
tmBool tmTri_isValid(tmTri *tri)
{
  tmDouble minAngle = 25.0 * PI_D / 180.;
  tmDouble maxAngle = 110.0 * PI_D / 180.;

  tmMesh   *mesh    = tri->mesh;
  tmSizeFun sizeFun = mesh->sizeFun;
  tmDouble  r       = sizeFun( tri->xy );

  List     *inCirc;
  ListNode *cur, *cur_bdry;
  tmQtree  *cur_qtree;

  tmNode   *n1 = tri->n1;
  tmNode   *n2 = tri->n2;
  tmNode   *n3 = tri->n3;

  /*-------------------------------------------------------
  | 0) Check if triangle is within the domain
  -------------------------------------------------------*/
  if ( tmMesh_objInside(mesh, tri, TM_TRI) == FALSE )
    return FALSE;

  /*-------------------------------------------------------
  | 1) Check if new triangle edges intersect with 
  |    with any existing triangle in its vicinity
  -------------------------------------------------------*/
  cur_qtree = mesh->tris_qtree;

  inCirc = tmQtree_getObjCirc(cur_qtree,
                              tri->xy,
                              r);
  if ( inCirc != NULL)
  {
    for (cur = inCirc->first; cur != NULL; cur = cur->next)
    {
      tmTri *t = (tmTri*)cur->value;

      if (t == tri)
        continue;

      if ( tmTri_triIntersect(tri,t) == TRUE )
      {
        List_destroy(inCirc);
        return FALSE;
      }
    }
  }

  if (inCirc != NULL)
    List_destroy(inCirc);

  /*-------------------------------------------------------
  | 2) Check if new triangle edges intersect with 
  |    with any existing boundary edge in its vicinity
  -------------------------------------------------------*/

  /*-------------------------------------------------------
  | 3) Check if new triangle edges intersect with 
  |    with any existing front edge in its vicinity
  -------------------------------------------------------*/

  /*-------------------------------------------------------
  | 3) Check if triangle quality is good enough
  -------------------------------------------------------*/
  if (tri->minAngle >= minAngle && tri->maxAngle <= maxAngle)
    return TRUE;

  tmPrint(" REJECTING T%d: (%d, %d, %d)",
      tri->index, tri->n1->index, tri->n2->index, tri->n3->index);
  tmPrint(" ANGLES: %.3f, %.3f", tri->minAngle, tri->maxAngle);

  return FALSE;

} /* tmTri_isValid() */


/**********************************************************
* Function: tmTri_edgeIntersect()
*----------------------------------------------------------
* Function to check wether a provided triangles intersects
* with an edge, defined by its vertices e1, e2
*----------------------------------------------------------
* @param *t: pointer to tmTri 
* @param *e1,e2: pointer to nodes defining the edge
**********************************************************/
tmBool tmTri_edgeIntersect(tmTri *t, tmNode *e1, tmNode *e2)
{
  tmDouble *n1_xy = t->n1->xy;
  tmDouble *n2_xy = t->n2->xy;
  tmDouble *n3_xy = t->n3->xy;

  tmDouble *e1_xy = e1->xy;
  tmDouble *e2_xy = e2->xy;

  if( INTERSECTION_IN_LINES(e1_xy, e2_xy, n1_xy, n2_xy) )
    return TRUE;
  
  if( INTERSECTION_IN_LINES(e1_xy, e2_xy, n2_xy, n3_xy) )
    return TRUE;

  if( INTERSECTION_IN_LINES(e1_xy, e2_xy, n3_xy, n1_xy) )
    return TRUE;

  return FALSE;


} /* tmTri_edgeIntersect() */


/**********************************************************
* Function: tmTri_triIntersectn()
*----------------------------------------------------------
* Function to check wether two provided triangles intersect
*----------------------------------------------------------
* @param *t1: pointer to first tmTri 
* @param *t2: pointer to second tmTri 
**********************************************************/
tmBool tmTri_triIntersect(tmTri *t1, tmTri *t2)
{
  tmNode *n1 = t2->n1;
  tmNode *n2 = t2->n2;
  tmNode *n3 = t2->n3;

  if ( tmTri_edgeIntersect(t1, n1, n2) == TRUE )
    return TRUE;

  if ( tmTri_edgeIntersect(t1, n2, n3) == TRUE )
    return TRUE;

  if ( tmTri_edgeIntersect(t1, n3, n1) == TRUE )
    return TRUE;
  
  return FALSE;

} /* tmTri_triIntersect() */
