#include "tmesh/tmNode.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmMesh.h"
#include "tmesh/tmBdry.h"

#include "tmesh/dbg.h"

#include "tmesh_cylinder.h"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

/*************************************************************
* 
*************************************************************/
static inline tmDouble size_fun( tmDouble xy[2] )
{
  tmDouble x0 = 200.0;
  tmDouble y0 = 200.0;

  tmDouble dx = x0 - xy[0];
  tmDouble dy = y0 - xy[1];
  tmDouble r2 = dx*dx + dy*dy;

  if (xy[0] < 500.0)
    return 40.0 - 39.0 * exp(-0.00001*r2);
  else
    return 25. + 0.01*xy[0];
}

/*************************************************************
* Function to create a mesh with a cylinder 
*************************************************************/
char *test_mesh_cylinder()
{
  tmDouble xy_min[2] = {    -1.0,   -1.0 };
  tmDouble xy_max[2] = {  2201.0,  411.0 };
  tmMesh *mesh = tmMesh_create(xy_min, xy_max, 25, size_fun);

  /*--------------------------------------------------------
  | exterior boundary
  --------------------------------------------------------*/
  tmDouble xy_e0[2] = {    0.0,    0.0 };
  tmDouble xy_e1[2] = { 2200.0,    0.0 };
  tmDouble xy_e2[2] = { 2200.0,  410.0 };
  tmDouble xy_e3[2] = {    0.0,  410.0 };

  tmNode *n_e0 = tmNode_create(mesh, xy_e0);
  tmNode *n_e1 = tmNode_create(mesh, xy_e1);
  tmNode *n_e2 = tmNode_create(mesh, xy_e2);
  tmNode *n_e3 = tmNode_create(mesh, xy_e3);

  tmBdry *bdry_ext = tmMesh_addBdry(mesh, FALSE, 0);

  tmEdge *e_e0 = tmBdry_edgeCreate(bdry_ext, n_e0, n_e1, 0);
  tmEdge *e_e1 = tmBdry_edgeCreate(bdry_ext, n_e1, n_e2, 1);
  tmEdge *e_e2 = tmBdry_edgeCreate(bdry_ext, n_e2, n_e3, 2);
  tmEdge *e_e3 = tmBdry_edgeCreate(bdry_ext, n_e3, n_e0, 3);


  /*--------------------------------------------------------
  | interior boundary
  --------------------------------------------------------*/
  tmDouble xy_center[2] = {  200.0,  200.0 };
  int      n_segs       = 60;
  tmDouble radius       = 50;

  tmNode   *n_circ[n_segs];
  tmEdge   *e_circ[n_segs];

  tmBdry *bdry_int = tmMesh_addBdry(mesh, TRUE, 1);

  int i;
  for (i=0; i<n_segs; i++)
  {
    tmDouble theta  = 2.0 * PI_D * (tmDouble)i / (tmDouble)n_segs;
    tmDouble x_circ = xy_center[0] + radius * sin( theta );
    tmDouble y_circ = xy_center[1] + radius * cos( theta );

    tmDouble xy_i[2] = { x_circ, y_circ };
    n_circ[i] = tmNode_create(mesh, xy_i);
  }

  for (i=0; i<n_segs; i++)
  {
    e_circ[i] = tmBdry_edgeCreate(bdry_int, n_circ[i], n_circ[(i+1)%n_segs], 4);
  }

  /*--------------------------------------------------------
  | Create mesh
  --------------------------------------------------------*/
  clock_t tic_1 = clock();
  tmMesh_ADFMeshing(mesh);
  tmMesh_delaunayFlip(mesh);
  clock_t tic_2 = clock();

  printf("> ----------------------------------------------\n");
  printf("> cylinder mesh performance test   \n");
  printf("> ----------------------------------------------\n");
  printf("> Number of elements: %d\n", mesh->no_tris);
  printf("> Meshing time      : %e sec\n", (double) (tic_2 - tic_1) / CLOCKS_PER_SEC );
  printf("> ----------------------------------------------\n");

  /*--------------------------------------------------------
  | Print the mesh data 
  --------------------------------------------------------*/
  tmMesh_printMesh(mesh);

  tmMesh_destroy(mesh);

  return NULL;

} /* test_mesh_cylinder() */
