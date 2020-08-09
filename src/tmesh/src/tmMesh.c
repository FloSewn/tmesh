#include "tmesh/tmTypedefs.h"
#include "tmesh/tmList.h"
#include "tmesh/tmMesh.h"
#include "tmesh/tmBdry.h"
#include "tmesh/tmFront.h"
#include "tmesh/tmEdge.h"
#include "tmesh/tmNode.h"
#include "tmesh/tmTri.h"
#include "tmesh/tmQtree.h"



/**********************************************************
* Function: tmMesh_create()
*----------------------------------------------------------
* Create a new tmMesh structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmMesh structure
**********************************************************/
tmMesh *tmMesh_create(tmDouble      xy_min[2], 
                      tmDouble      xy_max[2], 
                      int           qtree_max_obj,
                      tmDouble      globSize,
                      tmSizeFunUser sizeFunUser)
{
  tmMesh *mesh = (tmMesh*) calloc( 1, sizeof(tmMesh) );
  check_mem(mesh);

  /*-------------------------------------------------------
  | Mesh properties 
  -------------------------------------------------------*/
  mesh->qtree_max_obj = qtree_max_obj;

  mesh->xy_min[0] = xy_min[0];
  mesh->xy_min[1] = xy_min[1];

  mesh->xy_max[0] = xy_max[0];
  mesh->xy_max[1] = xy_max[1];

  mesh->areaBdry = 0.0;
  mesh->areaTris = 0.0;

  mesh->globSize = globSize;

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  mesh->nodes_head    = NULL;
  mesh->nodes_stack   = tmList_create();
  mesh->no_nodes      = 0;
  mesh->nodes_qtree   = tmQtree_create(mesh, TM_NODE);
  tmQtree_init(mesh->nodes_qtree, NULL, 0, xy_min, xy_max); 

  /*-------------------------------------------------------
  | Mesh boundary edges 
  -------------------------------------------------------*/
  mesh->bdry_stack         = tmList_create();
  mesh->no_bdrys           = 0;

  /*-------------------------------------------------------
  | Mesh front edges 
  -------------------------------------------------------*/
  mesh->front             = tmFront_create(mesh);

  /*-------------------------------------------------------
  | Mesh size function 
  -------------------------------------------------------*/
  mesh->sizeFun            = &tmMesh_sizeFun;
  mesh->sizeFunUser        = sizeFunUser;

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  mesh->edges_stack        = tmList_create();
  mesh->no_edges           = 0;
  mesh->edges_qtree        = tmQtree_create(mesh, TM_EDGE);
  tmQtree_init(mesh->edges_qtree, NULL, 0, xy_min, xy_max);

  /*-------------------------------------------------------
  | Stack to keep track of non-Delaunay edges
  -------------------------------------------------------*/
  mesh->delaunay_stack     = tmList_create();
  
  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  mesh->tris_head         = NULL;
  mesh->tris_stack        = tmList_create();
  mesh->no_tris           = 0;
  mesh->no_tris_delaunay  = 0;
  mesh->tris_qtree        = tmQtree_create(mesh, TM_TRI);
  tmQtree_init(mesh->tris_qtree, NULL, 0, xy_min, xy_max);


  return mesh;
error:
  return NULL;

} /* tmMesh_create() */

/**********************************************************
* Function: tmMesh_destroy()
*----------------------------------------------------------
* Destroys a tmMesh structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmMesh to destroy
*
* @return: Returns TRUE if method works
**********************************************************/
void tmMesh_destroy(tmMesh *mesh)
{
  tmListNode *cur, *nxt;

  /*-------------------------------------------------------
  | Free all boundary structures
  -------------------------------------------------------*/
  cur = nxt = mesh->bdry_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmBdry_destroy(cur->value);
    cur = nxt;
  }
#if (TM_DEBUG > 0)
  tmPrint("MESH BOUNDARIES FREE");
#endif

  /*-------------------------------------------------------
  | Free advancing front structure
  -------------------------------------------------------*/
  tmFront_destroy(mesh->front);
#if (TM_DEBUG > 0)
  tmPrint("MESH FRONT FREE");
#endif

  /*-------------------------------------------------------
  | Free all edges on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->edges_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmEdge_destroy(cur->value);
    cur = nxt;
  }
#if (TM_DEBUG > 0)
  tmPrint("MESH EDGES DESTROYED");
#endif

  /*-------------------------------------------------------
  | Free all tris on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->tris_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmTri_destroy(cur->value);
    cur = nxt;
  }
#if (TM_DEBUG > 0)
  tmPrint("MESH TRIS DESTROYED");
#endif

  /*-------------------------------------------------------
  | Free all nodes on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->nodes_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmNode_destroy(cur->value);
    cur = nxt;
  }
#if (TM_DEBUG > 0)
  tmPrint("MESH NODES DESTROYED");
#endif

  /*-------------------------------------------------------
  | Free all quadtree structures
  -------------------------------------------------------*/
  tmQtree_destroy(mesh->nodes_qtree);
#if (TM_DEBUG > 0)
  tmPrint("MESH NODES_QTREE FREE");
#endif

  tmQtree_destroy(mesh->edges_qtree);
#if (TM_DEBUG > 0)
  tmPrint("MESH EDGES_QTREE FREE");
#endif

  tmQtree_destroy(mesh->tris_qtree);
#if (TM_DEBUG > 0)
  tmPrint("MESH TRIS_QTREE FREE");
#endif

  /*-------------------------------------------------------
  | Free all list structures
  -------------------------------------------------------*/
  tmList_destroy(mesh->nodes_stack);
  tmList_destroy(mesh->edges_stack);
  tmList_destroy(mesh->tris_stack);
  tmList_destroy(mesh->bdry_stack);
  tmList_destroy(mesh->delaunay_stack);

  /*-------------------------------------------------------
  | Finally free mesh structure memory
  -------------------------------------------------------*/
  free(mesh);

} /* tmMesh_destroy() */


/**********************************************************
* Function: tmMesh_addNode()
*----------------------------------------------------------
* Function to add a tmNode to a tmMesh
*----------------------------------------------------------
* @return: tmNode index on the mesh's node stack
**********************************************************/
tmListNode *tmMesh_addNode(tmMesh *mesh, tmNode *node)
{
  tmListNode *node_pos;
  mesh->no_nodes += 1;
  tmList_push(mesh->nodes_stack, node);
  tmQtree_addObj(mesh->nodes_qtree, node);
  node_pos = tmList_last_node(mesh->nodes_stack);
  
  return node_pos;

} /* tmMesh_addNode() */


/**********************************************************
* Function: tmMesh_edgeCreate()
*----------------------------------------------------------
* Function to create a new edge for a tmMesh structure
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
* @param n1, n2: start/ending node of edge
* @param t1, t2: triangle to the left / right of the edge
**********************************************************/
tmEdge *tmMesh_edgeCreate(tmMesh *mesh, 
                          tmNode *n1, tmNode *n2,
                          tmTri  *t1, tmTri  *t2)
{
  tmEdge *edge = tmEdge_create(mesh, n1, n2, NULL, 2);

  /*--------------------------------------------------------
  | t1: Triangle to the left of this edge
  | t2: Triangle to the right of this edge
  --------------------------------------------------------*/
  edge->t1 = t1;
  edge->t2 = t2;

  /*--------------------------------------------------------
  | Connect edge to triangles
  | t1 is located to the left of this edge
  | t2 is located to the right of this edge
  --------------------------------------------------------*/
  if (t1 != NULL)
  {
    if (n1 == t1->n1) /* -> n2 == t1->n2 */
      t1->e3 = edge;
    else if (n1 == t1->n2) /* -> n2 == t1->n3 */
      t1->e1 = edge;
    else
      t1->e2 = edge;
  }

  if (t2 != NULL)
  {
    if (n1 == t2->n1) /* -> n2 == t2->n3 */
      t2->e2 = edge;
    else if (n1 == t2->n2) /* -> n2 == t2->n1 */
      t2->e3 = edge;
    else
      t2->e1 = edge;
  }

  /*--------------------------------------------------------
  | Check if edge meets local Delaunay constraint
  --------------------------------------------------------*/
  tmEdge_isDelaunay(edge);

  return edge;

} /* tmMesh_edgeCreate() */

/**********************************************************
* Function: tmMesh_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmMesh structure
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
* @param edge: edge to add 
**********************************************************/
tmListNode *tmMesh_addEdge(tmMesh *mesh, tmEdge *edge)
{
  tmListNode *edge_pos;

  mesh->no_edges += 1;
  tmList_push(mesh->edges_stack, edge);
  tmQtree_addObj(mesh->edges_qtree, edge);
  edge_pos = tmList_last_node(mesh->edges_stack);

  return edge_pos;

} /* tmMesh_addEdge() */

/**********************************************************
* Function: tmMesh_addTri()
*----------------------------------------------------------
* Function to add a tmTri to a tmMesh
*----------------------------------------------------------
* @return: tmTri index on the mesh's tri stack
**********************************************************/
tmListNode *tmMesh_addTri(tmMesh *mesh, tmTri *tri)
{
  tmListNode *tri_pos;

  mesh->no_tris  += 1;
  mesh->areaTris += tri->area;

  tmList_push(mesh->tris_stack, tri);
  tmQtree_addObj(mesh->tris_qtree, tri);
  tri_pos = tmList_last_node(mesh->tris_stack);
  
  return tri_pos;

} /* tmMesh_addTri() */


/**********************************************************
* Function: tmMesh_remNode()
*----------------------------------------------------------
* Function to remove a tmNode from a tmMesh
*----------------------------------------------------------
*
**********************************************************/
void tmMesh_remNode(tmMesh *mesh, tmNode *node)
{
  tmBool    qtree_rem;

  if ( node->mesh != mesh )
    log_warn("Can not remove node from mesh. Node not found.");

  /*-------------------------------------------------------
  | Remove node from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(mesh->nodes_qtree, node);
  mesh->no_nodes -= 1;

  /*-------------------------------------------------------
  | Remove node from stack
  -------------------------------------------------------*/
  tmList_remove(mesh->nodes_stack, node->stack_pos);

} /* tmMesh_remNode() */

/**********************************************************
* Function: tmMesh_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmMesh structure
*----------------------------------------------------------
*
**********************************************************/
void tmMesh_remEdge(tmMesh *mesh, tmEdge *edge)
{
  tmBool    qtree_rem;

  /*-------------------------------------------------------
  | Check if object is in the mesh
  -------------------------------------------------------*/
  if ( edge->mesh != mesh )
    log_warn("Can not remove edge from mesh. Edge not found.");

  /*-------------------------------------------------------
  | Remove edge from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(mesh->edges_qtree, edge);
  mesh->no_edges -= 1;

  /*-------------------------------------------------------
  | Remove edge from stack
  -------------------------------------------------------*/
  tmList_remove(mesh->edges_stack, edge->stack_pos);

  /*-------------------------------------------------------
  | Destroy edge -> removes also adjacency to edge nodes
  -------------------------------------------------------*/
  //tmEdge_destroy(edge);

} /* tmMesh_remEdge() */

/**********************************************************
* Function: tmMesh_remTri()
*----------------------------------------------------------
* Function to remove a tmTri from a tmMesh
*----------------------------------------------------------
*
**********************************************************/
void tmMesh_remTri(tmMesh *mesh, tmTri *tri)
{
  tmBool    qtree_rem;

  /*-------------------------------------------------------
  | Check if object is in the nodestack
  -------------------------------------------------------*/
  if ( tri->mesh != mesh )
    log_warn("Can not remove tri from mesh. Tri not found.");

  /*-------------------------------------------------------
  | Remove node from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(mesh->tris_qtree, tri);

  mesh->no_tris  -= 1;
  mesh->areaTris -= tri->area;

  /*-------------------------------------------------------
  | Remove node from stack
  -------------------------------------------------------*/
  tmList_remove(mesh->tris_stack, tri->stack_pos);

} /* tmMesh_remTri() */

/**********************************************************
* Function: tmMesh_addBdry()
*----------------------------------------------------------
* Function to add a boundary structure to a tmMesh
*----------------------------------------------------------
* @return: tmBdry pointer
**********************************************************/
tmBdry *tmMesh_addBdry(tmMesh *mesh,
                       tmBool  is_interior,
                       int     index)
{
  tmBdry *bdry = tmBdry_create(mesh, is_interior, index);
  mesh->no_bdrys += 1;
  tmList_push(mesh->bdry_stack, bdry);
  bdry->mesh_pos = tmList_last_node(mesh->bdry_stack);

  return bdry;

} /* tmMesh_addBdry() */

/**********************************************************
* Function: tmMesh_remBdry()
*----------------------------------------------------------
* Function to remove a boundary structure from a tmMesh
*----------------------------------------------------------
*
**********************************************************/
void tmMesh_remBdry(tmMesh *mesh, tmBdry *bdry)
{
  /*-------------------------------------------------------
  | Remove boundary from the mesh 
  -------------------------------------------------------*/
  mesh->no_bdrys -= 1;
  tmList_remove(mesh->bdry_stack, bdry->mesh_pos);

} /* tmMesh_remBdry() */


/**********************************************************
* Function: tmMesh_objInside()
*----------------------------------------------------------
* Function to check wether an object at position xy 
* is contained within the mesh boundary
*----------------------------------------------------------
* @param mesh
* @param xy
**********************************************************/
tmBool tmMesh_objInside(tmMesh   *mesh, tmDouble xy[2])
{
  tmListNode *cur;
  tmBdry *cur_bdry;

  tmBool is_inside = TRUE;

  for (cur = mesh->bdry_stack->first; 
       cur != NULL; cur = cur->next)
  {
    cur_bdry = (tmBdry*) cur->value;

    if (cur_bdry->is_interior == TRUE)
    {
      is_inside &= !(tmBdry_isInside(cur_bdry, xy));
    }
    else
    {
      is_inside &= tmBdry_isInside(cur_bdry, xy);
    }
  }

  return is_inside;

} /* tmMesh_objInside() */


/**********************************************************
* Function: tmMesh_printMesh()
*----------------------------------------------------------
* Fuction to print out the mesh data
*----------------------------------------------------------
* @param mesh: pointer to mesh structure
**********************************************************/
void tmMesh_printMesh(tmMesh *mesh) 
{
  tmListNode *cur, *cur_bdry;
  int node_index = 0;
  int edge_index = 0;
  int tri_index  = 0;
  int bdry_edge_index = 0;

  /*-------------------------------------------------------
  | Set node indices and print node coordinates
  -------------------------------------------------------*/
  fprintf(stdout,"NODES %d\n", mesh->no_nodes);
  for (cur = mesh->nodes_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmDouble *xy = ((tmNode*)cur->value)->xy;
    ((tmNode*)cur->value)->index = node_index;
    fprintf(stdout,"%d\t%9.5f\t%9.5f\n", node_index, xy[0], xy[1]);
    node_index += 1;
  }

  /*-------------------------------------------------------
  | print boundary edges
  -------------------------------------------------------*/
  for (cur_bdry = mesh->bdry_stack->first; 
       cur_bdry != NULL; cur_bdry = cur_bdry->next)
  {
    tmBdry *bdry = (tmBdry*)cur_bdry->value;

    fprintf(stdout,"BOUNDARY %d %d\n", 
        ((tmBdry*)cur_bdry->value)->index,
        ((tmBdry*)cur_bdry->value)->no_edges);

    for (cur = bdry->edges_stack->first; 
         cur != NULL; cur = cur->next)
    {
      tmIndex ind1 = ((tmEdge*)cur->value)->n1->index;
      tmIndex ind2 = ((tmEdge*)cur->value)->n2->index;
      tmIndex marker = ((tmEdge*)cur->value)->bdry_marker;
      ((tmEdge*)cur->value)->index = edge_index;
      fprintf(stdout,"%d\t%9d\t%9d\t%9d\n", edge_index, 
          ind1, ind2, marker);
      edge_index += 1;
    }
  }

  /*-------------------------------------------------------
  | print front edges
  -------------------------------------------------------*/
  edge_index = 0;
  fprintf(stdout,"FRONT %d\n", mesh->front->no_edges);
  for (cur = mesh->front->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmIndex ind1 = ((tmEdge*)cur->value)->n1->index;
    tmIndex ind2 = ((tmEdge*)cur->value)->n2->index;
    ((tmEdge*)cur->value)->index = edge_index;
    fprintf(stdout,"%d\t%9d\t%9d\n", edge_index, ind1, ind2);
    edge_index += 1;
  }

  /*-------------------------------------------------------
  | print triangles
  -------------------------------------------------------*/
  fprintf(stdout,"TRIANGLES %d\n", mesh->no_tris);
  for (cur = mesh->tris_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmTri *curTri = (tmTri*)cur->value;
    ((tmTri*)cur->value)->index = tri_index;

    fprintf(stdout,"%d\t%d\t%d\t%d\n", 
        tri_index, 
        curTri->n1->index,
        curTri->n2->index,
        curTri->n3->index);

    tri_index += 1;
  }

  /*-------------------------------------------------------
  | Print mesh edges and triangle neighbors
  -------------------------------------------------------*
  fprintf(stdout,"MESH EDGES %d\n", mesh->no_edges);
  edge_index = 0;
  for (cur = mesh->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmIndex ind1 = ((tmEdge*)cur->value)->n1->index;
    tmIndex ind2 = ((tmEdge*)cur->value)->n2->index;
    ((tmEdge*)cur->value)->index = edge_index;

    tmTri *t1 = ((tmEdge*)cur->value)->t1;
    tmTri *t2 = ((tmEdge*)cur->value)->t2;

    tmIndex i1 = -1;
    tmIndex i2 = -1;

    if (t1 != NULL)
      i1 = t1->index;
    if (t2 != NULL)
      i2 = t2->index;

    fprintf(stdout,"%d\t%9d\t%9d\t%9d\t%9d\n", 
        edge_index, ind1, ind2, i1, i2);
    edge_index += 1;
  }*/

  /*-------------------------------------------------------
  | print triangles neighbors
  -------------------------------------------------------*/
  fprintf(stdout,"NEIGHBORS %d\n", mesh->no_tris);
  tri_index = 0;
  for (cur = mesh->tris_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmTri *curTri = (tmTri*)cur->value;

    tmTri *t1 = ((tmTri*)cur->value)->t1;
    tmTri *t2 = ((tmTri*)cur->value)->t2;
    tmTri *t3 = ((tmTri*)cur->value)->t3;

    tmIndex i1 = -1;
    tmIndex i2 = -1;
    tmIndex i3 = -1;

    if (t1 != NULL)
    {
      i1 = t1->index == tri_index ? -1 : t1->index;
    }
    if (t2 != NULL)
    {
      i2 = t2->index == tri_index ? -1 : t2->index;
    }
    if (t3 != NULL)
    {
      i3 = t3->index == tri_index ? -1 : t3->index;
    }

    fprintf(stdout,"%d\t%d\t%d\t%d\n", 
        tri_index, i1, i2, i3);
    
    tri_index += 1;
  } 

} /* tmMesh_printMesh() */


/**********************************************************
* Function: tmMesh_adfMeshing()
*----------------------------------------------------------
* Function to perform the advancing front loop 
* until either no edges are available anymore or until 
* a final iteration is reached
*----------------------------------------------------------
* 
**********************************************************/
void tmMesh_ADFMeshing(tmMesh *mesh)
{
  int      n            = 0;
  int      progress     = 0;
  int      oldProgress  = 0;
  tmDouble area_inv     = 0.0;

  tmListNode *cur, *nxt;
  tmFront  *front = mesh->front;

  /*-------------------------------------------------------
  | Initialize size function for boundaries
  -------------------------------------------------------*/
  for (cur = mesh->bdry_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmBdry_initSizeFun( (tmBdry*) cur->value );
  }

  /*-------------------------------------------------------
  | Initialize the front from mesh boundaries
  -------------------------------------------------------*/
  tmFront_init(mesh);
  tmFront_refine(mesh);
  tmFront_sortEdges(mesh);

  /*-------------------------------------------------------
  | Compute mesh area
  -------------------------------------------------------*/
  tmMesh_calcArea(mesh);
  check(mesh->areaBdry > 0.0, 
      "Invalid mesh boundary. Domain area <= zero.");
  area_inv = 100. / mesh->areaBdry;

  /*-------------------------------------------------------
  | Main loop for finding creating triangles
  -------------------------------------------------------*/
  cur = nxt = front->edges_stack->first;
  while (n < front->no_edges && front->no_edges > 0)
  {
    /*-----------------------------------------------------
    | Print meshing progress
    -----------------------------------------------------*/
    progress = (int) (mesh->areaTris  * area_inv);
    if (((progress % 10) == 0) && (progress > oldProgress))
    {
      oldProgress = progress;
      tmPrint("ADVANCING FRONT PROGRESS: %2d%%", progress);
    }

    /*-----------------------------------------------------
    | Choose new base segment
    -----------------------------------------------------*/
    nxt = cur->next;
    tmEdge *curEdge = (tmEdge*)cur->value;
    
#if (TM_DEBUG > 1)
    tmPrint("-----------------------------------");
    tmPrint("FRONT-EDGE (%d/%d): (%d -> %d)", 
        n, front->no_edges,
        curEdge->n1->index, curEdge->n2->index);
#endif

    /*-----------------------------------------------------
    | Try to form new triangle with current base segment
    | -> go to next segment, if it failed
    -----------------------------------------------------*/
    if ( tmFront_advance(mesh, curEdge) == TRUE )
    {
      //tmFront_sortEdges(mesh);
      cur = front->edges_stack->first;
      n = 0;
    }
    else
    {
      cur = nxt;
      n += 1;
    }
  }

  /*-------------------------------------------------------
  | Compare mesh area to triangle areas
  -------------------------------------------------------*/
  if ( front->no_edges > 0 )
    log_err("The advancing front meshing was not successfull.");
  else
  {
    tmDouble err_area = fabs(mesh->areaTris - mesh->areaBdry) / mesh->areaBdry;
    check( err_area < 1e-5,
        "Mesh area %.5f does not equal to sum of triangle area %.5f", mesh->areaBdry, mesh->areaTris);
  }

  /*-------------------------------------------------------
  | Set triangle-triangle connectivity
  -------------------------------------------------------*/
  tmMesh_setTriNeighbors(mesh);

error:
  return;

} /* tmMesh_adfMeshing() */



/**********************************************************
* Function: tmMesh_delaunayFlip()
*----------------------------------------------------------
* Function to perform the flip algorithm on a triangulated
* mesh, in order to obtain a triangulation that is delaunay
*----------------------------------------------------------
* 
**********************************************************/
void tmMesh_delaunayFlip(tmMesh *mesh)
{
  /*-------------------------------------------------------
  | Create list of edges that are not locally delaunay
  -------------------------------------------------------*/
  int       n_flip = 0;
  tmListNode *cur;

  int n_edges  = mesh->edges_stack->count;
  int flip_max = n_edges * n_edges;

#if (TM_DEBUG > 1)
  tmPrint("-----------------------------------");
  tmPrint("NUMBER OF NON-DELAUNAY EDGES: %d",
      mesh->delaunay_stack->count);
  for (cur = mesh->delaunay_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmIndex i1 = ((tmEdge*)cur->value)->n1->index;
    tmIndex i2 = ((tmEdge*)cur->value)->n2->index;
    tmPrint("  EDGE (%d,%d)", i1, i2);
  }
#endif

  /*-------------------------------------------------------
  | Flip non-delaunay edges 
  -------------------------------------------------------*/
  while ( mesh->delaunay_stack->count > 0 && n_flip < flip_max)
  {
    n_flip   += 1;
    tmEdge_flipEdge( (tmEdge*)tmList_pop(mesh->delaunay_stack) );
  }

  /*-------------------------------------------------------
  | Set triangle-triangle connectivity
  -------------------------------------------------------*/
  tmMesh_setTriNeighbors(mesh);

#if (TM_DEBUG > 1)
  tmPrint("NUMBER OF DELAUNAY EDGE FLIPS: %d", n_flip);
  tmPrint("-----------------------------------");
#endif

error:
  return;

} /* tmMesh_delaunayFlip() */

/**********************************************************
* Function: tmMesh_setTriNeighbors()
*----------------------------------------------------------
* Sets for every triangle the connectivity to its neigbors
* This is done, when the advancing front meshing 
* has finished.
*----------------------------------------------------------
* @param mesh: the mesh structure
**********************************************************/
void tmMesh_setTriNeighbors(tmMesh *mesh)
{
  tmListNode *cur; 

  for (cur = mesh->tris_stack->first; 
       cur != NULL; cur = cur->next)
  {
    tmTri *tri = (tmTri*)cur->value;

    tri->t1 = tmTri_findTriNeighbor(tri->n2, tri->n3, tri);
    tri->t2 = tmTri_findTriNeighbor(tri->n3, tri->n1, tri);
    tri->t3 = tmTri_findTriNeighbor(tri->n1, tri->n2, tri);
  }

} /* tmMesh_setTriNeighbors() */

/**********************************************************
* Function: tmMesh_calcArea()
*----------------------------------------------------------
* Function to compute the area enclosed by the entire
* mesh domain
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
**********************************************************/
void tmMesh_calcArea(tmMesh *mesh)
{
  tmListNode *cur_bdry;

  tmDouble area = 0.0;

  for (cur_bdry = mesh->bdry_stack->first; 
       cur_bdry != NULL; cur_bdry = cur_bdry->next)
  {
    tmBdry *bdry = (tmBdry*)cur_bdry->value;
    tmBdry_calcArea(bdry);
    area += bdry->area;
  }

  mesh->areaBdry = area;

} /* tmMesh_calcArea() */

/**********************************************************
* Function: tmMesh_getTriFromCoords()
*----------------------------------------------------------
* Function returns a triangle which contains a given 
* point (x,y). If the point is located on the edge
* of two triangles, the first one that has been located
* on the tri-stack will be chosen.
*----------------------------------------------------------
* @param mesh: mesh for which the edge is defined
* @param xy: point coordinates
* @return: triangle, in which xy is contained.
**********************************************************/
tmTri* tmMesh_getTriFromCoords(tmMesh *mesh, tmDouble xy[2])
{
  tmListNode *cur;

  tmSizeFun sizeFun = mesh->sizeFun;
  tmDouble  r = TM_TRI_NODE_RANGE_FAC * sizeFun(mesh, xy);

  tmList *inCirc = tmQtree_getObjCirc(mesh->tris_qtree, xy, r);

  /*-------------------------------------------------------
  | For each found triangle, check if point (x,y) is 
  | located within it
  -------------------------------------------------------*/
  if ( inCirc != NULL)
  {
    for (cur = inCirc->first; cur != NULL; cur = cur->next)
    {
      tmTri    *t   = (tmTri*)cur->value;

      tmDouble *xy1 = t->n1->xy;
      tmDouble *xy2 = t->n2->xy;
      tmDouble *xy3 = t->n3->xy;

      tmBool left1 = IS_LEFTON( xy1, xy2, xy);
      tmBool left2 = IS_LEFTON( xy2, xy3, xy);
      tmBool left3 = IS_LEFTON( xy3, xy1, xy);

      tmBool n_in_tri =  left1 & left2 & left3;

      if (n_in_tri == TRUE)
      {
        tmList_destroy(inCirc);
        return t;
      }
    }
  }

  tmList_destroy(inCirc);
  return NULL;

} /* tmMesh_getTriFromCoords() */


/**********************************************************
* Function: tmMesh_refineLocal()
*----------------------------------------------------------
* Add a new node at a specified location in the mesh
* and subsequently re-triangulate the mesh in this area.
*----------------------------------------------------------
* @param mesh: the mesh structure
* @param xy:   coordinates of the new node to insert
*
**********************************************************/
void tmMesh_refineLocally(tmMesh *mesh, tmDouble xy[2])
{
  /*-------------------------------------------------------
  | Find triangle from that point 
  -------------------------------------------------------*/
  tmTri *tri = tmMesh_getTriFromCoords(mesh, xy);

  if (tri == NULL)
    return;

  tmNode *n1 = tri->n1;
  tmNode *n2 = tri->n2;
  tmNode *n3 = tri->n3;

  tmEdge *e1 = tri->e1;
  tmEdge *e2 = tri->e2;
  tmEdge *e3 = tri->e3;

  tmTri  *t1 = NULL;
  tmTri  *t2 = NULL;
  tmTri  *t3 = NULL;

  tmTri_destroy(tri);

  tmList *edges = tmList_create();

  /*-----------------------------------------------------
  | Add edges from tri neighbor 1
  -----------------------------------------------------*/
  if (n3 == e1->n1)
    t1 = e1->t1;
  else if (n3 == e1->n2)
    t1 = e1->t2;
  else
    log_err("Wrong edge-node connectivity.");

  if (t1 != NULL)
  {
    if (e1 == t1->e1) 
    {
      if (t1->e2 != NULL)
        tmList_push(edges, t1->e2);
      if (t1->e3 != NULL)
        tmList_push(edges, t1->e3);
    }
    else if (e1 == t1->e2)
    {
      if (t1->e1 != NULL)
        tmList_push(edges, t1->e1);
      if (t1->e3 != NULL)
        tmList_push(edges, t1->e3);
    }
    else if (e1 == t1->e3)
    {
      if (t1->e1 != NULL)
        tmList_push(edges, t1->e1);
      if (t1->e2 != NULL)
        tmList_push(edges, t1->e2);
    }
    else
      log_err("Wrong triangle-edge connectivity");

    /*---------------------------------------------------
    | Remove triangle 
    ---------------------------------------------------*/
    tmTri_destroy(t1);
  }
  /*-----------------------------------------------------
  | Add edge 1 if no tri neighbor 1 exists
  -----------------------------------------------------*/
  else
  {
    tmList_push(edges, e1);
  }

  /*-----------------------------------------------------
  | Get edges from tri neighbor 2
  -----------------------------------------------------*/
  if (n1 == e2->n1)
    t2 = e2->t1;
  else if (n1 == e2->n2)
    t2 = e2->t2;
  else
    log_err("Wrong edge-node connectivity.");

  if (t2 != NULL)
  {
    if (e2 == t2->e1) 
    {
      if (t2->e2 != NULL)
        tmList_push(edges, t2->e2);
      if (t2->e3 != NULL)
        tmList_push(edges, t2->e3);
    }
    else if (e2 == t2->e2)
    {
      if (t2->e1 != NULL)
        tmList_push(edges, t2->e1);
      if (t2->e3 != NULL)
        tmList_push(edges, t2->e3);
    }
    else if (e2 == t2->e3)
    {
      if (t2->e1 != NULL)
        tmList_push(edges, t2->e1);
      if (t2->e2 != NULL)
        tmList_push(edges, t2->e2);
    }
    else
      log_err("Wrong triangle-edge connectivity");

    /*---------------------------------------------------
    | Remove triangle 
    ---------------------------------------------------*/
    tmTri_destroy(t2);
  }
  /*-----------------------------------------------------
  | Add edge 2 if no tri neighbor 2 exists
  -----------------------------------------------------*/
  else
  {
    tmList_push(edges, e2);
  }


  /*-----------------------------------------------------
  | Get edges from tri neighbor 3
  -----------------------------------------------------*/
  if (n2 == e3->n1)
    t3 = e3->t1;
  else if (n2 == e3->n2)
    t3 = e3->t2;
  else
    log_err("Wrong edge-node connectivity.");

  if (t3 != NULL)
  {
    if (e3 == t3->e1) 
    {
      if (t3->e2 != NULL)
        tmList_push(edges, t3->e2);
      if (t3->e3 != NULL)
        tmList_push(edges, t3->e3);
    }
    else if (e3 == t3->e2)
    {
      if (t3->e1 != NULL)
        tmList_push(edges, t3->e1);
      if (t3->e3 != NULL)
        tmList_push(edges, t3->e3);
    }
    else if (e3 == t3->e3)
    {
      if (t3->e1 != NULL)
        tmList_push(edges, t3->e1);
      if (t3->e2 != NULL)
        tmList_push(edges, t3->e2);
    }
    else
      log_err("Wrong triangle-edge connectivity");

    /*---------------------------------------------------
    | Remove triangle 
    ---------------------------------------------------*/
    tmTri_destroy(t3);

  }
  /*-----------------------------------------------------
  | Add edge 3 if no tri neighbor 3 exists
  -----------------------------------------------------*/
  else
  {
    tmList_push(edges, e3);
  }

  /*-----------------------------------------------------
  | Add new node at location xy and create new triangles
  -----------------------------------------------------*/
  tmTri  *new_tri;
  tmTri  *first_tri, *cur_tri;

  tmNode *new_node = tmNode_create(mesh, xy);
  tmEdge *base     = tmList_pop(edges);
  tmNode *first_node;

  if (ORIENTATION(base->n1->xy, base->n2->xy, xy) == 1) 
  {
    new_tri = tmTri_create(mesh, base->n1, 
                           base->n2, new_node);
    first_node  = base->n2;
    base->t1    = new_tri;
    new_tri->e3 = base;
  }
  else
  {
    new_tri = tmTri_create(mesh, base->n2, 
                           base->n1, new_node);
    first_node = base->n1;
    base->t2   = new_tri;
    new_tri->e3 = base;
  }

  tmEdge_isDelaunay(base);

  first_tri = new_tri;
  cur_tri   = new_tri;

  tmListNode *cur;
  tmNode   *cur_node;
  tmEdge   *new_edge;

  while (edges->count > 0)
  {
    base  = tmList_pop(edges);

    if (ORIENTATION(base->n1->xy, base->n2->xy, xy) == 1) 
    {
      new_tri = tmTri_create(mesh, base->n1, 
                             base->n2, new_node);
      cur_node    = base->n2;
      base->t1    = new_tri;
      new_tri->e3 = base;
    }
    else
    {
      new_tri = tmTri_create(mesh, base->n2, 
                             base->n1, new_node);
      cur_node    = base->n1;
      base->t2    = new_tri;
      new_tri->e3 = base;
    }

    new_edge = tmMesh_edgeCreate(mesh,
                                 new_node, cur_node,
                                 new_tri, cur_tri);

    cur_tri = new_tri;
    tmEdge_isDelaunay(base);
    tmPrint("tmList count: %d", edges->count);
  }

  new_edge = tmMesh_edgeCreate(mesh,
                               new_node, first_node,
                               first_tri, cur_tri);

} /* tmMesh_insertMeshNode() */


/**********************************************************
* Function: tmMesh_sizeFun()
*----------------------------------------------------------
* Define a global size function for the mesh
*----------------------------------------------------------
* @param mesh: the mesh structure
* @param xy:   coordinates to evaluate the size function
*
**********************************************************/
tmDouble tmMesh_sizeFun(tmMesh *mesh, tmDouble xy[2])
{
  tmListNode *cur_b, *cur_e;
  tmBdry *bdry;
  tmEdge *edge;
  tmDouble rho0;

  if (mesh->sizeFunUser == NULL)
    rho0 = mesh->globSize;
  else
    rho0 = mesh->sizeFunUser(xy);

  for (cur_b = mesh->bdry_stack->first; 
       cur_b != NULL; cur_b = cur_b->next)
  {
    bdry = (tmBdry*) cur_b->value;

    for (cur_e = bdry->edges_stack->first; 
         cur_e != NULL; cur_e = cur_e->next)
    {
      edge = (tmEdge*) cur_e->value;
      const tmDouble dx  = 0.5*(edge->n1->xy[0]-xy[0]);
      const tmDouble dy  = 0.5*(edge->n1->xy[1]-xy[1]);
      const tmDouble r2  = dx*dx + dy*dy;
      const tmDouble rho = edge->n1->rho + edge->n1->k * r2;

      if ( rho < rho0) 
        rho0 = rho;
    }

  }

  return rho0;

} /* tmMesh_sizeFun() */
