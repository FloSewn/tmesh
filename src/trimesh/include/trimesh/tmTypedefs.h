#ifndef TRIMESH_TMTYPEDEFS_H
#define TRIMESH_TMTYPEDEFS_H

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "lcthw/dbg.h"
#include "lcthw/list.h"
#include "lcthw/darray.h"

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
* Basic structure labels
***********************************************************/
#define TM_NODE 0
#define TM_EDGE 1
#define TM_TRI  2

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
* Debugging
***********************************************************/
#define TM_DEBUG 1

/***********************************************************
* Constants used in code
***********************************************************/
#define PI_D 3.1415926535897932384626433832795

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



#endif /* TRIMESH_TMTYPEDEFS_H */
