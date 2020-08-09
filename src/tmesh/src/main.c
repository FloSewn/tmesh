#include "tmesh/tmParam.h"
#include "tmesh/tmNode.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmTri.h"
#include "tmesh/tmBdry.h"
#include "tmesh/tmMesh.h"
#include "tmesh/tmQtree.h"
#include "tmesh/tmFront.h"
#include "tmesh/tmList.h"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    tmPrint("tmesh <Input-File>");
    return 0;
  }

  char *input = argv[1];
  tmParam *file;
  
  int i,j;

  tmDouble  globSize;
  tmDouble *globBbox = NULL;
  int       qtreeSize;

  tmDouble (*nodes)[2] = NULL;
  int        nNodes;

  int     (*extBdryEdges)[2]  = NULL;
  int      *extBdryEdgeMarker = NULL;
  tmDouble *extBdryRefinement = NULL;
  int       nExtBdryEdges;
  int       extBdryMarker;

  int     (**intrEdges)[2] = NULL;
  int      **intrEdgeMarkers = NULL;
  tmDouble **intrEdgeRefinements = NULL;
  int       *nIntrEdges = NULL;
  int       *intrBdryMarkers = NULL;
  int        nIntrBdrys = 0;
    

  /*----------------------------------------------------------
  | Load parameter file and clear comments
  ----------------------------------------------------------*/
  struct bstrList *buf;

  file = tmParam_create( input );
  buf  = tmParam_popLinesWith(file->txtlist, "#");
  bstrListDestroy(file->txtlist);
  file->txtlist = buf;

  /*----------------------------------------------------------
  | Extract mandatory meshing parameters
  ----------------------------------------------------------*/
  check( tmParam_extractParam(file->txtlist, 
       "Global element size:", 1, &globSize) != 0,
      "Missing parameter <Global element size:>");

  check( tmParam_extractArray(file->txtlist,
        "Mesh bounding box:", 1, &globBbox)  != 0,
      "Missing parameter <Mesh bounding box:>");

  /*----------------------------------------------------------
  | Extract optional meshing parameters
  ----------------------------------------------------------*/
  if ( tmParam_extractParam(file->txtlist, 
       "Number of quadtree elements:", 0, &qtreeSize) == 0 )
  {
    qtreeSize = 100;
  }

  /*----------------------------------------------------------
  | Extract node definitions
  ----------------------------------------------------------*/
  tmParam_readNodeCoords(file->txtlist, &nodes, &nNodes);

  /*----------------------------------------------------------
  | Extract exterior boundary definition
  ----------------------------------------------------------*/
  tmParam_readExtBdryData(file->txtlist,
                          &extBdryEdges, 
                          &extBdryEdgeMarker, 
                          &extBdryRefinement, 
                          &nExtBdryEdges, 
                          &extBdryMarker);

  /*----------------------------------------------------------
  | Extract interior boundary definitions
  ----------------------------------------------------------*/
  tmParam_readIntBdryData(file->txtlist,
                          &intrEdges, 
                          &intrEdgeMarkers,
                          &intrEdgeRefinements,
                          &nIntrEdges, 
                          &intrBdryMarkers,
                          &nIntrBdrys);

  /*----------------------------------------------------------
  | Build the mesh
  ----------------------------------------------------------*/
  tmDouble xyMin[2] = { globBbox[0], globBbox[1] };
  tmDouble xyMax[2] = { globBbox[2], globBbox[3] };

  tmMesh *mesh = tmMesh_create(xyMin, xyMax, 
                               qtreeSize, 
                               globSize, NULL);


  /*----------------------------------------------------------
  | Add nodes to the mesh
  ----------------------------------------------------------*/
  tmNode **nodes_ptr = calloc(nNodes, sizeof(tmNode*));
  tmNode  *newNode;

  for (i = 0; i < nNodes; i++)
  {
    newNode = tmNode_create(mesh, nodes[i]);
    nodes_ptr[i] = newNode;
  }

  /*----------------------------------------------------------
  | Add exterior boundary to the mesh
  ----------------------------------------------------------*/
  tmEdge *bdryEdge;
  tmBdry *bdry_ext = tmMesh_addBdry(mesh, 
                                    FALSE, 
                                    extBdryMarker);

  for (i = 0; i < nExtBdryEdges; i++)
  {
    int i0 = extBdryEdges[i][0];
    int i1 = extBdryEdges[i][1];

    check( i0 < nNodes && i1 < nNodes,
        "Defined nodes do not fit to exterior boundary definition.");

    bdryEdge = tmBdry_edgeCreate(bdry_ext, 
                                 nodes_ptr[i0],
                                 nodes_ptr[i1],
                                 extBdryEdgeMarker[i],
                                 extBdryRefinement[i]);
  }


  /*----------------------------------------------------------
  | Add interior boundaries to the mesh
  ----------------------------------------------------------*/
  tmBdry *bdry_int;
  for (j = 0; j < nIntrBdrys; j++)
  {
    bdry_int = tmMesh_addBdry(mesh, 
                              TRUE, 
                              intrBdryMarkers[j]);

    for (i = 0; i < nIntrEdges[j]; i++)
    {
      int i0 = intrEdges[j][i][0];
      int i1 = intrEdges[j][i][1];

      check( i0 < nNodes && i1 < nNodes,
          "Defined nodes do not fit to interior boundary with marker %d.",
          intrBdryMarkers[j]);

      bdryEdge = tmBdry_edgeCreate(bdry_int, 
                                   nodes_ptr[i0],
                                   nodes_ptr[i1],
                                   intrEdgeMarkers[j][i],
                                   intrEdgeRefinements[j][i]);
    }
  }

  /*--------------------------------------------------------
  | Create mesh
  --------------------------------------------------------*/
  clock_t tic_1 = clock();
  tmMesh_ADFMeshing(mesh);
  clock_t tic_2 = clock();

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);


  tmPrint("----------------------------------------------\n");
  tmPrint("TMesh finished\n");
  tmPrint("----------------------------------------------\n");
  tmPrint("Number of elements: %d\n", mesh->no_tris);
  tmPrint("Meshing time      : %e sec\n", 
      (double) (tic_2 - tic_1) / CLOCKS_PER_SEC );
  tmPrint("----------------------------------------------\n");

  tmMesh_destroy(mesh);



  free(globBbox);

  free(nodes);
  free(nodes_ptr);

  free(extBdryEdges);
  free(extBdryEdgeMarker);
  free(extBdryRefinement);

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

  tmParam_destroy( file );

  return 0;



error:

  free(globBbox);

  free(nodes);
  
  free(extBdryEdges);
  free(extBdryEdgeMarker);
  free(extBdryRefinement);

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

  tmParam_destroy( file );

  return 1;

}
