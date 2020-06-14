#ifndef TRIMESH_TMTYPEDEFS_H
#define TRIMESH_TMTYPEDEFS_H

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "lcthw/dbg.h"
#include "lcthw/list.h"
#include "lcthw/list_algos.h"
#include "lcthw/darray.h"

/***********************************************************
* Flag to use double precision -> undefine to use float
***********************************************************/
#define TM_USE_DOUBLE

/***********************************************************
* Own type definitions in order to allow to switch from 
* switch to short and from double to long in case of 
* special memory requirements
***********************************************************/
#define tmDouble double
#define tmIndex  int
#define tmBool   int
#define TRUE     1
#define FALSE    0

/***********************************************************
* Basic structure labels
***********************************************************/
#define TM_NODE 0
#define TM_EDGE 1
#define TM_TRI  2

/***********************************************************
* Basic structure typedefs
***********************************************************/
typedef struct tmNode  tmNode;
typedef struct tmEdge  tmEdge;
typedef struct tmTri   tmTri;
typedef struct tmBdry  tmBdry;
typedef struct tmFront tmFront;
typedef struct tmQtree tmQtree;
typedef struct tmMesh  tmMesh;

/***********************************************************
* Function pointers
***********************************************************/
typedef tmDouble (*tmSizeFun) (tmDouble xy[2]);

/***********************************************************
* Debugging Layers
* 0 -> No output
***********************************************************/
#define TM_DEBUG 1

#ifdef TM_DEBUG 
#define tmPrint(M, ...) fprintf(stdout, "> " M "\n",\
    ##__VA_ARGS__)
#else
#define tmPrint(M, ...)
#endif


/***********************************************************
* Constants used in code
***********************************************************/
#define PI_D 3.1415926535897932384626433832795
#define TM_MIN_SIZE        1.0E-8
#define TM_QTREE_MAX_LAYER 100


/***********************************************************
* MESHING PARAMETERS
***********************************************************/
#define TM_NEW_NODE_DIST_FAC  ( 1.25 )
#define TM_NODE_EDGE_DIST_FAC ( 0.40 )
#define TM_TRI_RANGE_FAC      ( 1.50 )
#define TM_BDRY_REFINE_FAC    ( 1.00 )
#define TM_NODE_NBR_DIST_FAC  ( 0.90 )
#define TM_TRI_MIN_ANGLE      ( ( 10.0*PI_D)/180.0 )
#define TM_TRI_MAX_ANGLE      ( (170.0*PI_D)/180.0 )
#define TM_TRI_MIN_QUALITY    ( 0.05 )

/***********************************************************
* Minimum and maximum allowed coordinates
***********************************************************/
#define TM_X_MIN -1.0E7
#define TM_Y_MIN -1.0E7
#define TM_X_MAX  1.0E7
#define TM_Y_MAX  1.0E7

#define SMALL     1.0E-13

/***********************************************************
* Helper functions
***********************************************************/
#ifndef SQR
#define SQR(x) ( (x) * (x) ) 
#endif

#ifndef MAX
#define MAX(a, b) ( (a) > (b) ? (a) : (b) ) 
#endif

#ifndef MIN
#define MIN(a, b) ( (a) < (b) ? (a) : (b) ) 
#endif

#ifndef MAX0
#define MAX0(a) ( (a) > 0 ? (a) : 0 ) 
#endif

#ifndef MIN0
#define MIN0(a) ( (a) < 0 ? (a) : 0 ) 
#endif

#ifndef ABS
#define ABS(a) ( (a) < 0 ? -1 * (a) : (a) ) 
#endif

#ifndef EQ
#define EQ(a, b) ( ABS((a)- (b)) < SMALL ? 1 : 0 )
#endif


/***********************************************************
* Geometric functions
***********************************************************/
#define IN_ON_BBOX(n, min, max) \
  (  ( (n)[0] >= (min)[0] )     \
  && ( (n)[1] >= (min)[1] )     \
  && ( (n)[0] <= (max)[0] )     \
  && ( (n)[1] <= (max)[1] )     )

#define IN_BBOX(n, min, max) \
  (  ( (n)[0] > (min)[0] )   \
  && ( (n)[1] > (min)[1] )   \
  && ( (n)[0] < (max)[0] )   \
  && ( (n)[1] < (max)[1] )   )

#define ON_BBOX(n, min, max) \
  (  ( EQ( (n)[0], (min)[0] ) && ( (n)[1] >= (min)[1] ) && ( (n)[1] <= (max)[1] ) ) \
  || ( EQ( (n)[0], (max)[0] ) && ( (n)[1] >= (min)[1] ) && ( (n)[1] <= (max)[1] ) ) \
  || ( EQ( (n)[1], (min)[1] ) && ( (n)[0] >= (min)[0] ) && ( (n)[0] <= (max)[0] ) ) \
  || ( EQ( (n)[1], (max)[1] ) && ( (n)[0] >= (min)[0] ) && ( (n)[0] <= (max)[0] ) ) )

#define BBOX_OVERLAP(pqmin, pqmax, rsmin, rsmax) \
  (  ( (pqmin)[0] <= (rsmax)[0] )                 \
  && ( (rsmin)[0] <= (pqmax)[0] )                 \
  && ( (pqmin)[1] <= (rsmax)[1] )                 \
  && ( (rsmin)[1] <= (pqmax)[1] ) )                


/*----------------------------------------------------------
| check the orientation of a node tuple (p,q,r)
----------------------------------------------------------*/
static inline int ORIENTATION(tmDouble p[2], 
                              tmDouble q[2], 
                              tmDouble r[2])
{
  tmDouble area2 = ( ( p[0] - r[0] ) * ( q[1] - r[1] )
                   - ( q[0] - r[0] ) * ( p[1] - r[1] ) );

  /* Colinearity of all nodes */
  if ( (area2 * area2) < SMALL )
    return 0;

  /* Counter clockwise orienation */
  if ( area2 > 0.0 )
    return 1;

  return 2;
}

/*----------------------------------------------------------
| check if node r is located to the left of (p,q) 
----------------------------------------------------------*/
static inline tmBool IS_LEFT(tmDouble p[2], 
                             tmDouble q[2], 
                             tmDouble r[2])
{
  if ( ORIENTATION(p, q, r) == 1 )
    return TRUE;
  return FALSE;
}

/*----------------------------------------------------------
| check if node r is located to the left of (p,q) or on it
----------------------------------------------------------*/
static inline tmBool IS_LEFTON(tmDouble p[2], 
                               tmDouble q[2], 
                               tmDouble r[2])
{
  if ( ORIENTATION(p, q, r) == 2 )
    return FALSE;
  return TRUE;
}

/*----------------------------------------------------------
| check if node r is located to the right of (p,q) 
----------------------------------------------------------*/
static inline tmBool IS_RIGHT(tmDouble p[2], 
                              tmDouble q[2], 
                              tmDouble r[2])
{
  if ( ORIENTATION(p, q, r) == 2 )
    return TRUE;
  return FALSE;
}

/*----------------------------------------------------------
| check if node r is located to the right of (p,q) or on it
----------------------------------------------------------*/
static inline tmBool IS_RIGHTON(tmDouble p[2], 
                                tmDouble q[2], 
                                tmDouble r[2])
{
  if ( ORIENTATION(p, q, r) == 1 )
    return FALSE;
  return TRUE;
}

/*----------------------------------------------------------
| Function returns the squared normal distance between an 
| edge and a node.
| The edge is defined through its vertices (v,w) 
| The node is defined through its coordinates p
| 
| Source:
|   https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
----------------------------------------------------------*/
static inline tmDouble EDGE_NODE_DIST2(tmDouble v[2],
                                       tmDouble w[2],
                                       tmDouble p[2])
{
  const tmDouble dx_e = w[0]-v[0];
  const tmDouble dy_e = w[1]-v[1];
  const tmDouble l2 = dx_e*dx_e + dy_e*dy_e;

  const tmDouble dotP = ( (p[0]-v[0]) * dx_e
                        + (p[1]-v[1]) * dy_e ) / l2;

  const tmDouble t = MAX(0.0, MIN(1.0, dotP));

  const tmDouble proj_x = v[0] + t * dx_e;
  const tmDouble proj_y = v[1] + t * dy_e;

  const tmDouble dx = proj_x - p[0];
  const tmDouble dy = proj_y - p[1];

  const tmDouble dist2 = dx*dx + dy*dy;
  
  return dist2;
}

/*----------------------------------------------------------
| Check if an object is contained in a provided list
----------------------------------------------------------*/
static inline tmBool OBJ_IN_LIST(void *obj, List *list)
{
  ListNode *cur;
  for (cur = list->first; cur != NULL; cur = cur->next)
    if ( cur->value == obj )
      return TRUE;

  return FALSE;
error:
  return -1;
}

/*----------------------------------------------------------
| Check if r lies within a segment (p,q)
----------------------------------------------------------*/
static inline tmBool IN_SEGMENT(tmDouble p[2], 
                                tmDouble q[2], 
                                tmDouble r[2])
{
  if ( ORIENTATION(p, q, r) != 0 )
    return FALSE;

  tmDouble bbox_min[2] = { MIN(p[0], q[0]), 
                           MIN(p[1], q[1]) };
  tmDouble bbox_max[2] = { MAX(p[0], q[0]), 
                           MAX(p[1], q[1]) };

  const tmDouble dx_e = q[0]-p[0];
  const tmDouble dy_e = q[1]-p[1];
  const tmDouble l2   = dx_e*dx_e + dy_e*dy_e;
  const tmDouble t    = ( (r[0]-p[0]) * dx_e
                        + (r[1]-p[1]) * dy_e ) / l2;

  if ( t > 0.0 && t < 1.0 )
    return TRUE;

  return FALSE;
}

/*----------------------------------------------------------
| Check if r lies within a segment (p,q) or on its endpoints
----------------------------------------------------------*/
static inline tmBool IN_ON_SEGMENT(tmDouble p[2], 
                                   tmDouble q[2], 
                                   tmDouble r[2])
{
  if ( ORIENTATION(p, q, r) != 0 )
    return FALSE;

  tmDouble bbox_min[2] = { MIN(p[0], q[0]), 
                           MIN(p[1], q[1]) };
  tmDouble bbox_max[2] = { MAX(p[0], q[0]), 
                           MAX(p[1], q[1]) };

  const tmDouble dx_e = q[0]-p[0];
  const tmDouble dy_e = q[1]-p[1];
  const tmDouble l2   = dx_e*dx_e + dy_e*dy_e;
  const tmDouble t    = ( (r[0]-p[0]) * dx_e
                        + (r[1]-p[1]) * dy_e ) / l2;

  if ( t >= 0.0 && t <= 1.0 )
    return TRUE;

  return FALSE;
}

/*----------------------------------------------------------
| Check if two line segments (p1,q1), (p2,q2) are crossing
| 
| * Returns True, if segments intersect at any point but 
|   their endings
| * Returns True, if one line contains a part of the other
| * Returns False, if both lines share both end points
| * Returns False in all other cases
| 
| -> Used for triangle edge intersection
----------------------------------------------------------*/
static inline tmBool INTERSECTION_IN_LINES(tmDouble p1[2], 
                                           tmDouble q1[2], 
                                           tmDouble p2[2], 
                                           tmDouble q2[2])
{
  int o1 = ORIENTATION(p1, q1, p2);
  int o2 = ORIENTATION(p1, q1, q2);
  int o3 = ORIENTATION(p2, q2, p1);
  int o4 = ORIENTATION(p2, q2, q1);

  if (  ( (o1 == 1 && o2 == 2) || (o1 == 2 && o2 == 1) ) 
     && ( (o3 == 1 && o4 == 2) || (o3 == 2 && o4 == 1) ) )
  {
    return TRUE;
  }

  /* (p1, q1) and p2 are colinear and p2 lies on segment (p1, q1) */
  if ( (o1 == 0) && ( IN_SEGMENT(p1, q1, p2) == TRUE ) )
    return TRUE;

  /* (p1, q1) and q2 are colinear and q2 lies on segment (p1, q1) */
  if ( (o2 == 0) && ( IN_SEGMENT(p1, q1, q2) == TRUE ) ) 
    return TRUE;

  /* (p2, q2) and p1 are colinear and p1 lies on segment (p2, q2) */
  if ( (o3 == 0) && ( IN_SEGMENT(p2, q2, p1) == TRUE ) ) 
    return TRUE;

  /* (p2, q2) and q1 are colinear and q1 lies on segment (p2, q2) */
  if ( (o4 == 0) && ( IN_SEGMENT(p2, q2, q1) == TRUE )) 
    return TRUE;

  return FALSE;

}


#endif /* TRIMESH_TMTYPEDEFS_H */
