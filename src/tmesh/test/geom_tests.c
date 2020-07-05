#include "tmesh/tmTypedefs.h"

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

  /*----------------------------------------------------------
  | In segment
  ----------------------------------------------------------*/
  tmDouble s1[2]  = {  1.0,  1.0  };
  tmDouble s2[2]  = {  4.0,  1.0  };
  tmDouble s3[2]  = {  2.0,  1.0  };
  tmDouble s4[2]  = {  5.0,  1.0  };

  mu_assert(ORIENTATION(s1, s2, s3) == 0,
      "ORIENTATION() gives wrong results.");

  mu_assert(IN_SEGMENT(s1, s2, s3) == TRUE,
      "IN_SEGMENT() gives wrong results.");

  mu_assert(IN_SEGMENT(s1, s2, s4) == FALSE,
      "IN_SEGMENT() gives wrong results.");

  mu_assert(IN_ON_SEGMENT(s1, s2, s1) == TRUE,
      "IN_ON_SEGMENT() gives wrong results.");

  mu_assert(IN_ON_SEGMENT(s1, s2, s4) == FALSE,
      "IN_ON_SEGMENT() gives wrong results.");


  /*----------------------------------------------------------
  | Line intersection
  ----------------------------------------------------------*/
  tmDouble n1[2]  = {  1.0,  1.0  };
  tmDouble n2[2]  = {  4.0,  1.0  };
  tmDouble n3[2]  = {  2.0,  2.0  };
  tmDouble n4[2]  = {  6.0,  2.0  };
  tmDouble n5[2]  = {  8.0,  2.0  };
  tmDouble n6[2]  = {  2.0,  5.0  };
  tmDouble n7[2]  = {  6.0,  4.0  };
  tmDouble n8[2]  = {  7.0,  5.0  };
  tmDouble n9[2]  = {  8.0,  5.0  };
  tmDouble n10[2] = {  9.0,  5.0  };
  tmDouble n11[2] = {  2.0,  8.0  };
  tmDouble n12[2] = {  6.0,  7.0  };
  tmDouble n13[2] = {  6.0,  9.0  };

  mu_assert(INTERSECTION_IN_LINES(n1, n2, n3, n4) == FALSE,
      "INTERSECTION_IN_LINES() gives wrong results.");
  mu_assert(INTERSECTION_IN_LINES(n6, n7, n6, n13) == FALSE,
      "INTERSECTION_IN_LINES() gives wrong results.");

  mu_assert(INTERSECTION_IN_LINES(n6, n13, n11, n12) == TRUE,
      "INTERSECTION_IN_LINES() gives wrong results.");
  mu_assert(INTERSECTION_IN_LINES(n5, n9, n8, n10) == TRUE,
      "INTERSECTION_IN_LINES() gives wrong results.");

  return NULL;
} /* test_geometric_functions() */
