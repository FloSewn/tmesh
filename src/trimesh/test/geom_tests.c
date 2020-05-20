#include "trimesh/tmTypedefs.h"

#include "lcthw/minunit.h"
#include "lcthw/dbg.h"

#include <stdio.h>
#include <stdlib.h>

/*************************************************************
* Unit test function for geometric functions
*************************************************************/
char *test_geometric_functions()
{
  tmDouble bbox_min[2] = { -1.23, -2.34 };
  tmDouble bbox_max[2] = {  2.34,  1.23 };

  tmDouble xy_1[2] = { 0.1,  0.1 };
  tmDouble xy_2[2] = {-5.1,  0.1 };
  tmDouble xy_3[2] = { 2.34,-1.1 };

  tmDouble bbox_1_min[2] = { 1.0, -3.0 };
  tmDouble bbox_1_max[2] = { 4.0,  1.1 };

  tmDouble bbox_2_min[2] = { 4.0, 1.0 };
  tmDouble bbox_2_max[2] = { 5.0, 2.0 };

  tmDouble bbox_3_min[2] = {-1.1, -2.2 };
  tmDouble bbox_3_max[2] = { 2.2,  1.1 };

  mu_assert( IN_ON_BBOX(xy_1, bbox_min, bbox_max) == TRUE,
      "IN_ON_BBOX() gives wrong results.");
  mu_assert( IN_ON_BBOX(xy_2, bbox_min, bbox_max) == FALSE,
      "IN_ON_BBOX() gives wrong results.");
  mu_assert( IN_ON_BBOX(xy_3, bbox_min, bbox_max) == TRUE,
      "IN_ON_BBOX() gives wrong results.");

  mu_assert( IN_BBOX(xy_1, bbox_min, bbox_max) == TRUE,
      "IN_BBOX() gives wrong results.");
  mu_assert( IN_BBOX(xy_2, bbox_min, bbox_max) == FALSE,
      "IN_BBOX() gives wrong results.");
  mu_assert( IN_BBOX(xy_3, bbox_min, bbox_max) == FALSE,
      "IN_BBOX() gives wrong results.");

  mu_assert( ON_BBOX(xy_1, bbox_min, bbox_max) == FALSE,
      "ON_BBOX() gives wrong results.");
  mu_assert( ON_BBOX(xy_2, bbox_min, bbox_max) == FALSE,
      "ON_BBOX() gives wrong results.");
  mu_assert( ON_BBOX(xy_3, bbox_min, bbox_max) == TRUE,
      "ON_BBOX() gives wrong results.");


  mu_assert( BBOX_OVERLAP(bbox_1_min, bbox_1_max, bbox_min, bbox_max) == TRUE,
      "BBOX_OVERLAP() gives wrong results.");
  mu_assert( BBOX_OVERLAP(bbox_2_min, bbox_2_max, bbox_min, bbox_max) == FALSE,
      "BBOX_OVERLAP() gives wrong results.");
  mu_assert( BBOX_OVERLAP(bbox_3_min, bbox_3_max, bbox_min, bbox_max) == TRUE,
      "BBOX_OVERLAP() gives wrong results.");


  return NULL;
} /* test_geometric_functions() */
