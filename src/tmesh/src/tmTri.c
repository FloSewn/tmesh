#include "tmesh/tmTypedefs.h"
#include "tmesh/tmMesh.h"
#include "tmesh/tmTri.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmNode.h"
#include "tmesh/tmQtree.h"

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
* Function: tmTri_calcTriQuality()
*----------------------------------------------------------
* Compute the overall optimality coefficient of a potential
* triangle
* factor -> 1: Good Triangle 
* factor -> 0: Bad Triangle
*----------------------------------------------------------
* @param tri: triangle structure
**********************************************************/
static void tmTri_calcTriQuality(tmTri *tri);


/**********************************************************
* Function: tmTri_splitTriangle()
*----------------------------------------------------------
* Splits a triangle into subtriangles. Different split
* variations are possible, according to a given split-type
*
*----------------------------------------------------------
* @param tri:   triangle to split
* @param type:  splitting type
**********************************************************/
static tmTri *tmTri_splitTriangle(tmTri *tri,
                                  int    type);


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

  tri->circ_xy[0] = Ux + p[0];
  tri->circ_xy[1] = Uy + p[1];
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
*
**********************************************************/
static void tmTri_calcTriQuality(tmTri *tri)
{
  tmDouble e1 = tri->edgeLen[0];
  tmDouble e2 = tri->edgeLen[1];
  tmDouble e3 = tri->edgeLen[2];

  tmDouble d1 = tri->mesh->sizeFun(tri->n1->xy);
  tmDouble d2 = tri->mesh->sizeFun(tri->n2->xy);
  tmDouble d3 = tri->mesh->sizeFun(tri->n3->xy);

  tmDouble delta_1 = 0.5 * (d1+d2);
  tmDouble delta_2 = 0.5 * (d2+d3);
  tmDouble delta_3 = 0.5 * (d3+d1);

  tmDouble f1_1 = e1 / (delta_1 + SMALL);
  tmDouble f1_2 = e2 / (delta_2 + SMALL);
  tmDouble f1_3 = e3 / (delta_3 + SMALL);

  tmDouble f2_1 = delta_1 / (e1 + SMALL);
  tmDouble f2_2 = delta_2 / (e2 + SMALL);
  tmDouble f2_3 = delta_3 / (e3 + SMALL);

  tmDouble q_1  = MIN(f1_1, f2_1);
  tmDouble q_2  = MIN(f1_2, f2_2);
  tmDouble q_3  = MIN(f1_3, f2_3);

  tri->quality = q_1 * q_2 * q_3 * tri->shapeFac;

} /* tmTri_calcTriQuality() */


/**********************************************************
* Function: tmTri_splitTriangle()
*----------------------------------------------------------
* Splits a triangle into subtriangles. Different split
* variations are possible, according to a given split-type
*
*----------------------------------------------------------
* @param tri:   triangle to split
* @param type:  splitting type
**********************************************************/
static tmTri *tmTri_splitTriangle(tmTri *tri,
                                  int    type)
{

} /* tmTri_splitTriangle() */




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
  | Find tri neighbors and set current triangle as new 
  | neighbor for them. 
  |
  |                  n3
  |                 /  \
  |             T2 /    \ T1
  |               /      \
  |              n1-------n2
  |                  T3
  |
  -------------------------------------------------------*/
  tri->t1 = NULL; 
  tri->t2 = NULL; 
  tri->t3 = NULL; 

  /*-------------------------------------------------------
  | triangle edges -> are set later, during the 
  | advancing front update (when mesh edges are created)
  -------------------------------------------------------*/
  tri->e1 = NULL;
  tri->e2 = NULL;
  tri->e3 = NULL;

  /*-------------------------------------------------------
  | Add this triangle to the nodes triangle lists
  -------------------------------------------------------*/
  List_push(n1->tris, tri);
  tri->n1_pos = List_last_node(n1->tris);
  n1->n_tris += 1;

  List_push(n2->tris, tri);
  tri->n2_pos = List_last_node(n2->tris);
  n2->n_tris += 1;

  List_push(n3->tris, tri);
  tri->n3_pos = List_last_node(n3->tris);
  n3->n_tris += 1;

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
  tri->quality    = 0.0;
  tri->edgeLen[0] = 0.0;
  tri->edgeLen[1] = 0.0;
  tri->edgeLen[2] = 0.0;
  tri->circ_xy[0] = 0.0;
  tri->circ_xy[1] = 0.0;
  tri->circ_r     = 0.0;
  tri->is_delaunay = FALSE;


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
  tmTri_calcTriQuality(tri);

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
  | Remove triangle from its adjacent nodes 
  -------------------------------------------------------*/
  List_remove(tri->n1->tris, tri->n1_pos);
  tri->n1->n_tris -= 1;

  List_remove(tri->n2->tris, tri->n2_pos);
  tri->n2->n_tris -= 1;

  List_remove(tri->n3->tris, tri->n3_pos);
  tri->n3->n_tris -= 1;

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
* Function: tmTri_findNbrTriFromEdge()
*----------------------------------------------------------
* Searches for a neighboring triangle, which is adjacent
* to an edge defined by two vertices (n1, n2)
* The arrangement of n1 and n2 plays no role.
*----------------------------------------------------------
* @param n1,n2: nodes defining an edge, for which a
*               triangle must be found
* @param tri:   triangle for which a neighbor will be found
*               if one has been found, it will be included
*               to its neighbors
**********************************************************/
tmTri *tmTri_findTriNeighbor(tmNode *n1, 
                             tmNode *n2,
                             tmTri  *tri)
{
  ListNode *cur;
  tmTri    *t_nb;
  List     *tris_nb = n1->tris;

  if (tris_nb->count == 0)
    return NULL;

  for (cur = tris_nb->first; cur != NULL; cur = cur->next)
  {
    t_nb = (tmTri*)cur->value;

    if (n2 == t_nb->n1)
    {
      if (n1 == t_nb->n2)
        t_nb->t3 = tri;
      else /* n1 == t_nb->n3 */
        t_nb->t2 = tri;
      return t_nb;
    }

    if (n2 == t_nb->n2)
    {
      if (n1 == t_nb->n1)
        t_nb->t3 = tri;
      else /* n1 == t_nb->n3 */
        t_nb->t1 = tri;
      return t_nb;
    }

    if (n2 == t_nb->n3)
    {
      if (n1 == t_nb->n1)
        t_nb->t2 = tri;
      else /* n1 == t_nb->n2 */
        t_nb->t1 = tri;
      return t_nb;
    }
  }

  return NULL;

} /* tmTri_findNbrTriFromEdge() */

/**********************************************************
* Function: tmTri_isValid()
*----------------------------------------------------------
* Function to check wether a provided triangle is valid
*----------------------------------------------------------
* @param *tri: pointer to a tmTri 
**********************************************************/
tmBool tmTri_isValid(tmTri *tri)
{
  tmDouble minAngle = TM_TRI_MIN_ANGLE;
  tmDouble maxAngle = TM_TRI_MAX_ANGLE;

  tmMesh   *mesh    = tri->mesh;
  tmSizeFun sizeFun = mesh->sizeFun;
  tmDouble  r       = TM_TRI_RANGE_FAC * tri->circ_r;

  tmDouble  fac     = TM_NODE_EDGE_DIST_FAC;
  tmDouble  dist    = sizeFun( tri->xy ) * fac;
  tmDouble  dist2   = dist * dist;

  List     *inCirc;
  ListNode *cur, *cur_bdry;
  tmQtree  *cur_qtree;

  tmNode   *n1 = tri->n1;
  tmNode   *n2 = tri->n2;
  tmNode   *n3 = tri->n3;

#if (TM_DEBUG > 1)
  tmPrint("FORMING POT. TRIANGLE: (%d, %d, %d)",
      n1->index, n2->index, n3->index);
#endif

  /*-------------------------------------------------------
  | 0) Check if triangle is within the domain
  -------------------------------------------------------*/
  if ( tmMesh_objInside(mesh, tri->xy) == FALSE )
  {
#if (TM_DEBUG > 1)
    tmPrint(" -> REJECTED: TRIANGLE OUTSIDE OF DOMAIN");
#endif
    return FALSE;
  }

  /*-------------------------------------------------------
  | 0) Check correct orientation of triangle
  -------------------------------------------------------*/
  if (ORIENTATION(n1->xy, n2->xy, n3->xy) != 1)
  {
#if (TM_DEBUG > 1)
    tmPrint(" -> REJECTED: WRONG TRI-ORIENTATION");
#endif
    return FALSE;
  }

  /*-------------------------------------------------------
  | 1) Check if new triangle edges intersect with 
  |    with any existing triangle in its vicinity
  -------------------------------------------------------*/
  cur_qtree = mesh->tris_qtree;
  inCirc    = tmQtree_getObjCirc(cur_qtree, tri->xy, r);

  if ( inCirc != NULL)
    for (cur = inCirc->first; cur != NULL; cur = cur->next)
    {
      tmTri *t = (tmTri*)cur->value;

      if (t == tri)
        continue;

      if ( tmTri_triIntersect(tri,t) == TRUE )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: TRIANGLE INTERSECTS TRI-EDGE");
#endif
        List_destroy(inCirc);
        return FALSE;
      }

#if (TM_DEBUG > 1)
      tmPrint("NO INTERSECTION (%d, %d, %d) AND (%d, %d, %d)",
          tri->n1->index, tri->n2->index, tri->n3->index,
          t->n1->index, t->n2->index, t->n3->index);
#endif
    }

  if (inCirc != NULL)
    List_destroy(inCirc);

  /*-------------------------------------------------------
  | 2) Check if new triangle or its edges intersect  
  |    with any existing node in its vicinity
  -------------------------------------------------------*/
  cur_qtree = mesh->nodes_qtree;
  inCirc    = tmQtree_getObjCirc(cur_qtree, tri->xy, r);

  if ( inCirc != NULL)
  {
    for (cur = inCirc->first; cur != NULL; cur = cur->next)
    {
      tmNode *n = (tmNode*)cur->value;

      /* Neglect nodes that are part of the triangle */
      if ( n == n1 || n == n2 || n == n3 )
        continue;

      /* Neglect nodes that are not set active 
       * -> used to filter out potential new node 
       *    from advancing front                     */
      if (n->is_active == FALSE)
        continue;

      /* Neglect nodes that are not on the front */
      if (n->on_front == FALSE)
        continue;

      /* Check that current node is not within the triangle */
      if ( tmTri_nodeIntersect(tri, n) == TRUE )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: TRIANGLE INTERSECTS NODE %d",
            n->index);
#endif
        List_destroy(inCirc);
        return FALSE;
      }

      /* Check that points on the front are not too close to
       * triangle edges */
      if ( EDGE_NODE_DIST2(n1->xy, n2->xy, n->xy) < dist2 )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: TRI-EDGE TOO CLOSE TO NODE %d",
            n->index);
#endif
        List_destroy(inCirc);
        return FALSE;
      }
      if ( EDGE_NODE_DIST2(n2->xy, n3->xy, n->xy) < dist2 )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: TRI-EDGE TOO CLOSE TO NODE %d",
            n->index);
#endif
        List_destroy(inCirc);
        return FALSE;
      }
      if ( EDGE_NODE_DIST2(n3->xy, n1->xy, n->xy) < dist2 )
      {
#if (TM_DEBUG > 1)
        tmPrint(" -> REJECTED: TRI-EDGE TOO CLOSE TO NODE %d",
            n->index);
#endif
        List_destroy(inCirc);
        return FALSE;
      }
    }
  }

  if (inCirc != NULL)
    List_destroy(inCirc);

  /*-------------------------------------------------------
  | 4) Check if triangle angles is good enough
  -------------------------------------------------------*/
  if (tri->minAngle <= minAngle || tri->maxAngle >= maxAngle)
  {
#if (TM_DEBUG > 1)
    tmPrint(" -> REJECTED: INVALID TRIANGLE ANGLES");
#endif
    return FALSE;
  }

  /*-------------------------------------------------------
  | 5) Check if triangle quality is good enough
  -------------------------------------------------------*/
  if (tri->quality <= TM_TRI_MIN_QUALITY )
  {
#if (TM_DEBUG > 1)
    tmPrint(" -> REJECTED: INVALID TRIANGLE QUALITY");
    if ( tri->quality < 0.0 )
      tmPrint(" -> ERROR: NEGAITE TRI-QUALITY OBTAINED");
#endif
    return FALSE;
  }

  return TRUE;

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

/**********************************************************
* Function: tmTri_nodeIntersect()
*----------------------------------------------------------
* Function to check wether a provided triangles intersects
* with a node
*----------------------------------------------------------
* @param *t: pointer to tmTri 
* @param *n: pointer to tmNode
**********************************************************/
tmBool tmTri_nodeIntersect(tmTri *t, tmNode *n)
{
  if (n == t->n1 || n == t->n2 || n == t->n3)
    return FALSE;

  tmDouble *xy1 = t->n1->xy;
  tmDouble *xy2 = t->n2->xy;
  tmDouble *xy3 = t->n3->xy;

  /* Check if node is contained within triangle */
  tmBool left1 = IS_LEFTON( xy1, xy2, n->xy);
  tmBool left2 = IS_LEFTON( xy2, xy3, n->xy);
  tmBool left3 = IS_LEFTON( xy3, xy1, n->xy);
  tmBool n_in_tri =  left1 & left2 & left3;

  if (n_in_tri == TRUE)
    return TRUE;

  return FALSE;


} /* tmTri_nodeIntersect() */

