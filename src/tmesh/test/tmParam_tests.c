#include <tmesh/minunit.h>
#include <tmesh/bstrlib.h>
#include <tmesh/tmParam.h>
#include "tmParam_tests.h"

//const char *testfile = "/datadisk/Code/C-Code/tmesh/share/files/example.para";
const char *testfile = "/datadisk/Code/C-Code/tmesh/share/files/comment_test.para";

/*************************************************************
* Unit test function to handle creation and 
* destruction of bstrings
*************************************************************/
char *test_tmParam_readfile()
{
  int i;
  bstring *ptr  = NULL;
  tmParam *file = tmParam_create( testfile );

  /*----------------------------------------------------------
  | Return all lines without '#' Comment specifier
  ----------------------------------------------------------*/
  struct bstrList *test_1 = tmParam_popLinesWith(file->txtlist, "#");

  ptr            = test_1->entry;
  bstring bcmp_1 = bfromcstr( "NODES" );

  for (i = 0; i < test_1->qty; i++)
    mu_assert( binstr(ptr[i], 0, bcmp_1) == BSTR_ERR,
      "<tmParam_popLinesWith> failed.");

  /*----------------------------------------------------------
  | Return all lines with 'Define nodes' specifier
  ----------------------------------------------------------*/
  struct bstrList *test_2 = tmParam_getLinesWith(file->txtlist, 
                                               "Define nodes:");

  ptr            = test_2->entry;
  bstring bcmp_2 = bfromcstr( "Define nodes:" );

  for (i = 0; i < test_2->qty; i++)
    mu_assert(binstr(ptr[i], 0, bcmp_2) != BSTR_ERR, 
        "<tmParam_getLinesWith> failed.");

  /*----------------------------------------------------------
  | Return integer / float / bstring parameter
  ----------------------------------------------------------*/
  int int_param, check;
  double dbl_param;
  bstring bstr_param;

  check = tmParam_extractParam(file->txtlist, 
       "Number of quadtree elements:", 0, &int_param);
  mu_assert( check == 1, "<tmParam_extractParam> failed.");
  mu_assert( int_param == 50, "<tmParam_extractParam> failed.");

  check = tmParam_extractParam(file->txtlist, 
       "Global element size:", 1, &dbl_param);
  mu_assert( check == 1, "<tmParam_extractParam> failed.");
  mu_assert( dbl_param == 0.5, 
      "<tmParam_extractParam> failed.");

  check = tmParam_extractParam(file->txtlist, 
       "Mesh bounding box:", 2, &bstr_param);
  mu_assert( check == 1, "<tmParam_extractParam> failed.");
  mu_assert( strcmp( " -10.0, -10.0, 20.0, 20.0", bstr_param->data) == 0,
       "<tmParam_extractParam> failed.");

  /*----------------------------------------------------------
  | Return array from parameter
  ----------------------------------------------------------*/
  int *int_array;
  check = tmParam_extractArray(file->txtlist,
      "Mesh bounding box:", 0, &int_array);
  mu_assert( check == 1, "<tmParam_extractArray> failed.");
  mu_assert( int_array[0] == -10 , "<tmParam_extractArray> failed.");
  mu_assert( int_array[2] ==  20 , "<tmParam_extractArray> failed.");
  free(int_array);

  double *dbl_array;
  check = tmParam_extractArray(file->txtlist,
      "Mesh bounding box:", 1, &dbl_array);
  mu_assert( check == 1, "<tmParam_extractArray> failed.");
  mu_assert( dbl_array[0] == -10.0 , "<tmParam_extractArray> failed.");
  mu_assert( dbl_array[2] ==  20.0 , "<tmParam_extractArray> failed.");
  free(dbl_array);

  /* THIS IS NOT WORKING YET! 
  struct bstrList *str_array;
  check = tmParam_extractArray(file->txtlist,
      "Mesh bounding box:", 2, &str_array);
  mu_assert( check == 1, "<tmParam_extractArray> failed.");
  mu_assert( str_array->qty == 4, "<tmParam_extractArray> failed.");
  bstrListDestroy(str_array); */

  /*----------------------------------------------------------
  | Read node coordinates
  ----------------------------------------------------------*/
  tmDouble (*nodes)[2];
  int nNodes;
  tmParam_readNodeCoords(file->txtlist,
      &nodes, &nNodes);
  mu_assert( nodes[0][0] == 1.0,
      "<tmParam_readNodeCoords> failed.");
  mu_assert( nodes[0][1] == 1.0,
      "<tmParam_readNodeCoords> failed.");
  mu_assert( nodes[5][0] == 2.0,
      "<tmParam_readNodeCoords> failed.");
  mu_assert( nodes[5][1] == 4.0,
      "<tmParam_readNodeCoords> failed.");
  free(nodes);

  /*----------------------------------------------------------
  | Read exterior boundary edges
  ----------------------------------------------------------*/
  int (*bdryEdges)[2];
  int  *bdryEdgeMarker;
  tmDouble *bdryRefinement;
  int   nBdryEdges;
  int   bdryMarker;

  tmParam_readExtBdryData(file->txtlist,
      &bdryEdges, &bdryEdgeMarker, &bdryRefinement, 
      &nBdryEdges, &bdryMarker);
  mu_assert( bdryMarker == 1,
      "<tmParam_readExtBdryData> failed.");
  mu_assert( nBdryEdges == 4,
      "<tmParam_readExtBdryData> failed.");
  mu_assert( bdryEdges[1][1] == 2,
      "<tmParam_readExtBdryData> failed.");
  mu_assert( bdryEdges[2][0] == 2,
      "<tmParam_readExtBdryData> failed.");
  mu_assert( bdryEdges[0][0] == 0,
      "<tmParam_readExtBdryData> failed.");
  mu_assert( bdryEdgeMarker[0] == 0,
      "<tmParam_readExtBdryData> failed.");
  mu_assert( bdryEdgeMarker[2] == 0,
      "<tmParam_readExtBdryData> failed.");
  mu_assert( bdryRefinement[2] == 1.4,
      "<tmParam_readExtBdryData> failed.");
  free(bdryEdges);
  free(bdryEdgeMarker);
  free(bdryRefinement);



  /*----------------------------------------------------------
  | Read exterior boundary edges
  ----------------------------------------------------------*/
  int (**intrEdges)[2];
  int **intrEdgeMarkers;
  tmDouble **intrEdgeRefinements;
  int *nIntrEdges;
  int *intrBdryMarkers;
  int  nIntrBdrys;

  tmParam_readIntBdryData(file->txtlist,
      &intrEdges, &intrEdgeMarkers,
      &intrEdgeRefinements,
      &nIntrEdges, &intrBdryMarkers,
      &nIntrBdrys);

  /*
  int j;
  for (j = 0; j < nIntrBdrys; j++)
  {
    tmPrint("INTERIOR BOUNDARY %d: %d EDGES, MARKER %d",
        j, nIntrEdges[j], intrBdryMarkers[j]);

    for (i = 0; i < nIntrEdges[j]; i++)
      tmPrint("  %d: (%d, %d), %d, %.3f",
          i, intrEdges[j][i][0], intrEdges[j][i][1], 
          intrEdgeMarkers[j][i], intrEdgeRefinements[j][i]);
  }
  */

  for (i = 0; i < nIntrBdrys; i++)
  {
    free(intrEdges[i]);
    free(intrEdgeMarkers[i]);
    free(intrEdgeRefinements[i]);
  }
  free(intrEdges);
  free(intrEdgeMarkers);
  free(intrEdgeRefinements);
  free(nIntrEdges);
  free(intrBdryMarkers);




  bdestroy(bstr_param);
  tmParam_destroy( file );
  bdestroy( bcmp_1 );
  bdestroy( bcmp_2 );
  bstrListDestroy( test_1 );
  bstrListDestroy( test_2 );

  return NULL;
}


/*************************************************************
* Read a file and clear all strings behind a comment 
* identifier
*************************************************************/
char *test_tmParam_comments()
{
  int i;

  tmParam *file = tmParam_create( testfile );

  /*----------------------------------------------------------
  | Remove all comments from the file
  ----------------------------------------------------------*/
  bstring comment_fltr = bfromcstr("#");
  struct bstrList *test_1 = tmParam_removeComments(file->txtlist,
                                                   comment_fltr->data);
  bstring *fl_ptr = test_1->entry;

  for (i = 0; i < test_1->qty; i++) 
  {
    mu_assert( binstr(fl_ptr[i], 0, comment_fltr) == BSTR_ERR,
        "<tmParam_removeComments> failed.");
  }

  /*----------------------------------------------------------
  | Free memory
  ----------------------------------------------------------*/
  bstrListDestroy( test_1 );
  bdestroy( comment_fltr );
  tmParam_destroy( file );

  return NULL;
} 
