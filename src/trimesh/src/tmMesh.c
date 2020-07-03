#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmBdry.h"
#include "trimesh/tmFront.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmTri.h"
#include "trimesh/tmQtree.h"



/**********************************************************
* Function: tmMesh_create()
*----------------------------------------------------------
* Create a new tmMesh structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmMesh structure
**********************************************************/
tmMesh *tmMesh_create(tmDouble  xy_min[2], 
                      tmDouble  xy_max[2], 
                      int       qtree_max_obj,
                      tmSizeFun sizeFun)
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

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  mesh->nodes_head    = NULL;
  mesh->nodes_stack   = List_create();
  mesh->no_nodes      = 0;
  mesh->nodes_qtree   = tmQtree_create(mesh, TM_NODE);
  tmQtree_init(mesh->nodes_qtree, NULL, 0, xy_min, xy_max); 

  /*-------------------------------------------------------
  | Mesh boundary edges 
  -------------------------------------------------------*/
  mesh->bdry_stack         = List_create();
  mesh->no_bdrys           = 0;

  /*-------------------------------------------------------
  | Mesh front edges 
  -------------------------------------------------------*/
  mesh->front             = tmFront_create(mesh);

  /*-------------------------------------------------------
  | Mesh size function 
  -------------------------------------------------------*/
  mesh->sizeFun           = sizeFun;

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  mesh->edges_stack        = List_create();
  mesh->no_edges           = 0;
  mesh->edges_qtree        = tmQtree_create(mesh, TM_EDGE);
  tmQtree_init(mesh->edges_qtree, NULL, 0, xy_min, xy_max);
  
  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  mesh->tris_head         = NULL;
  mesh->tris_stack        = List_create();
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
  ListNode *cur, *nxt;

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
  List_destroy(mesh->nodes_stack);
  List_destroy(mesh->edges_stack);
  List_destroy(mesh->tris_stack);
  List_destroy(mesh->bdry_stack);

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
ListNode *tmMesh_addNode(tmMesh *mesh, tmNode *node)
{
  ListNode *node_pos;
  mesh->no_nodes += 1;
  List_push(mesh->nodes_stack, node);
  tmQtree_addObj(mesh->nodes_qtree, node);
  node_pos = List_last_node(mesh->nodes_stack);
  
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
  edge->is_local_delaunay = tmEdge_isLocalDelaunay(edge);

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
ListNode *tmMesh_addEdge(tmMesh *mesh, tmEdge *edge)
{
  ListNode *edge_pos;

  mesh->no_edges += 1;
  List_push(mesh->edges_stack, edge);
  tmQtree_addObj(mesh->edges_qtree, edge);
  edge_pos = List_last_node(mesh->edges_stack);

  return edge_pos;

} /* tmMesh_addEdge() */

/**********************************************************
* Function: tmMesh_addTri()
*----------------------------------------------------------
* Function to add a tmTri to a tmMesh
*----------------------------------------------------------
* @return: tmTri index on the mesh's tri stack
**********************************************************/
ListNode *tmMesh_addTri(tmMesh *mesh, tmTri *tri)
{
  ListNode *tri_pos;
  mesh->no_tris += 1;
  List_push(mesh->tris_stack, tri);
  tmQtree_addObj(mesh->tris_qtree, tri);
  tri_pos = List_last_node(mesh->tris_stack);
  
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
  List_remove(mesh->nodes_stack, node->stack_pos);

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
  List_remove(mesh->edges_stack, edge->stack_pos);

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
  mesh->no_tris -= 1;

  /*-------------------------------------------------------
  | Remove node from stack
  -------------------------------------------------------*/
  List_remove(mesh->tris_stack, tri->stack_pos);

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
  List_push(mesh->bdry_stack, bdry);
  bdry->mesh_pos = List_last_node(mesh->bdry_stack);

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
  List_remove(mesh->bdry_stack, bdry->mesh_pos);

} /* tmMesh_remBdry() */


/**********************************************************
* Function: tmMesh_objInside()
*----------------------------------------------------------
* Function to check wether an object is contained within
* the mesh boundary
*----------------------------------------------------------
* @param mesh
* @param obj
* @param obj_type
**********************************************************/
tmBool tmMesh_objInside(tmMesh *mesh, 
                        void   *obj, 
                        int     obj_type)
{
  ListNode *cur;
  tmBdry *cur_bdry;

  tmBool is_inside = TRUE;

  for (cur = mesh->bdry_stack->first; 
       cur != NULL; cur = cur->next)
  {
    cur_bdry = (tmBdry*) cur->value;

    if (cur_bdry->is_interior == TRUE)
    {
      is_inside &= !(tmBdry_isInside(cur_bdry, 
                                     obj, obj_type));
    }
    else
    {
      is_inside &= tmBdry_isInside(cur_bdry, 
                                   obj, obj_type);
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
  ListNode *cur, *cur_bdry;
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
  -------------------------------------------------------*/
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
  }

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
      i1 = t1->index;
    if (t2 != NULL)
      i2 = t2->index;
    if (t3 != NULL)
      i3 = t3->index;

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
  int n = 0;

  ListNode *cur, *nxt;

  tmFront *front = mesh->front;

  /*-------------------------------------------------------
  | Initialize the front from mesh boundaries
  -------------------------------------------------------*/
  tmFront_init(mesh);
  tmFront_sortEdges(mesh);

  /*-------------------------------------------------------
  | Main loop for finding creating triangles
  -------------------------------------------------------*/
  cur = nxt = front->edges_stack->first;
  while (n < front->no_edges && front->no_edges > 0)
  {
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
      tmFront_sortEdges(mesh);
      cur = front->edges_stack->first;
      n = 0;
    }
    else
    {
      cur = nxt;
      n += 1;
    }
  }

  if ( front->no_edges > 0 )
    log_err("The advancing front meshing was not successfull.");

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
  List     *edges  = List_create();
  ListNode *cur;

  for (cur = mesh->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    if ( ((tmEdge*)cur->value)->is_local_delaunay == FALSE )
      List_push(edges, cur->value);
  }

#if (TM_DEBUG > 1)
  tmPrint("NUMBER OF NON-DELAUNAY EDGES: %d",
      edges->count);
  for (cur = edges->first; 
       cur != NULL; cur = cur->next)
  {
    tmIndex i1 = ((tmEdge*)cur->value)->n1->index;
    tmIndex i2 = ((tmEdge*)cur->value)->n2->index;
    tmPrint("  EDGE (%d,%d)", i1, i2);
  }
#endif

  /*-------------------------------------------------------
  | Flip edges 
  -------------------------------------------------------*/
  int flip_max = 100;
  while ( edges->count > 0 && n_flip < flip_max)
  {
    n_flip   += 1;
    tmEdge *e = (tmEdge*)List_pop(edges);

    tmTri *t1 = e->t1;
    tmTri *t2 = e->t2;

    tmNode *n1 = e->n1;
    tmNode *n2 = e->n2;

    /*-----------------------------------------------------
    | Find quadrilateral nodes 
    | -> p1, p2
    | Find quadrilateral edges 
    | -> e11, e12, e21, e22
    -----------------------------------------------------*/
    tmNode *p1, *p2;
    tmEdge *e11, *e12, *e21, *e22;

    if (n1 == t1->n1) /* e = t1->e3 */
    {
      p2   = t1->n3;
      e11  = t1->e1;
      e12  = t1->e2;
      check(e  == t1->e3, "Wrong triangle-edge definition.");
      check(n2 == t1->n2, "Wrong triangle-node definition.");
    }
    else if (n1 == t1->n2) /* e = t1->e1 */
    {
      p2   = t1->n1;
      e11  = t1->e2;
      e12  = t1->e3;
      check(e  == t1->e1, "Wrong triangle-edge definition.");
      check(n2 == t1->n3, "Wrong triangle-node definition.");
    }
    else if (n1 == t1->n3)/* e = t1->e2 */
    {
      p2   = t1->n2;
      e11  = t1->e3;
      e12  = t1->e1;
      check(e  == t1->e2, "Wrong triangle-edge definition.");
      check(n2 == t1->n1, "Wrong triangle-node definition.");
    }
    else
      log_err("Wrong triangle definition.");

    if (n1 == t2->n1) /* e = t2->e2 */
    {
      p1   = t2->n2;
      e21  = t2->e3;
      e22  = t2->e1;
      check(e == t2->e2, "Wrong triangle-edge definition.");
      check(n2 == t2->n3, "Wrong triangle-node definition.");
    }
    else if (n1 == t2->n2) /* e = t2->e3 */
    {
      p1  = t2->n3;
      e21 = t2->e1;
      e22 = t2->e2;
      check(e == t2->e3, "Wrong triangle-edge definition.");
      check(n2 == t2->n1, "Wrong triangle-node definition.");
    }
    else if (n1 == t2->n3) /* e = t2->e1 */
    {
      p1  = t2->n1;
      e21 = t2->e2;
      e22 = t2->e3;
      check(e == t2->e1, "Wrong triangle-edge definition.");
      check(n2 == t2->n2, "Wrong triangle-node definition.");
    }
    else
      log_err("Wrong triangle definition.");

    /*-----------------------------------------------------
    | Create new triangles t1*, t2* and remove old ones
    | t1* = (n1, p1, p2)
    | t2* = (n2, p2, p1)
    -----------------------------------------------------*/
    tmTri_destroy(t1);
    tmTri_destroy(t2);
    t1 = tmTri_create(mesh, n1, p1, p2);
    t2 = tmTri_create(mesh, n2, p2, p1);

    /*-----------------------------------------------------
    | Create new edge e* = (p1, p2) and remove old one
    -----------------------------------------------------*/
#if (TM_DEBUG > 1)
    tmPrint("FLIPPING EDGE (%d,%d) to (%d,%d)",
        e->n1->index, e->n2->index, p1->index, p2->index);
#endif
    tmEdge_destroy(e);
    e = tmMesh_edgeCreate(mesh, p1, p2, t1, t2);

    /*-----------------------------------------------------
    | Update triangles t1*, t2* in quad-edges e11, e12,...
    -----------------------------------------------------*/
    /* e11 = (n2, p2) */
    if (e11->n1 == n2)
      e11->t1 = t2;
    else if (e11->n1 == p2)
      e11->t2 = t2;
    else
      log_err("Wrong edge definition. (%d,%d) -> (%d,%d)",
          n2->index, p2->index, e11->n1->index, e11->n2->index);

    /* e12 = (p2, n1) */
    if (e12->n1 == p2)
      e12->t1 = t1;
    else if (e12->n1 == n1)
      e12->t2 = t1;
    else
      log_err("Wrong edge definition. (%d,%d) -> (%d,%d)",
          n2->index, p2->index, e11->n1->index, e11->n2->index);

    /* e21 = (n1, p1) */
    if (e21->n1 == n1)
      e21->t1 = t1;
    else if (e21->n1 == p1)
      e21->t2 = t1;
    else
      log_err("Wrong edge definition. (%d,%d) -> (%d,%d)",
          n2->index, p2->index, e11->n1->index, e11->n2->index);

    /* e22 = (p1,n2) */
    if (e22->n1 == p1)
      e22->t1 = t2;
    else if (e22->n1 == n2)
      e22->t2 = t2;
    else
      log_err("Wrong edge definition. (%d,%d) -> (%d,%d)",
          n2->index, p2->index, e11->n1->index, e11->n2->index);

    /*-----------------------------------------------------
    | Add quad edges e11, e12,.. to triangles t1*, t2*
    -----------------------------------------------------*/
    t1->e1 = e;
    t1->e2 = e12;
    t1->e3 = e21;

    t2->e1 = e;
    t2->e2 = e22;
    t2->e3 = e11;

    /*-----------------------------------------------------
    | Update t1*, t2* in quad tri-neighbors t11, t12,...
    | -> Already done on triangle generation
    -----------------------------------------------------*/

    /*-----------------------------------------------------
    | Check if edges e11, e12,.. are locally delaunay
    | if not -> add them to the list
    -----------------------------------------------------*/
    e11->is_local_delaunay = tmEdge_isLocalDelaunay(e11);
    e12->is_local_delaunay = tmEdge_isLocalDelaunay(e12);
    e21->is_local_delaunay = tmEdge_isLocalDelaunay(e21);
    e22->is_local_delaunay = tmEdge_isLocalDelaunay(e22);

    if (e11->is_local_delaunay == FALSE)
    {
      List_push(edges, e11);
#if (TM_DEBUG > 1)
      tmPrint("EDGE (%d,%d) is put on Delaunay flip stack",
          e11->n1->index, e11->n2->index);
#endif
    }

    if (e12->is_local_delaunay == FALSE)
    {
      List_push(edges, e12);
#if (TM_DEBUG > 1)
      tmPrint("EDGE (%d,%d) is put on Delaunay flip stack",
          e12->n1->index, e12->n2->index);
#endif
    }

    if (e21->is_local_delaunay == FALSE)
    {
      List_push(edges, e21);
#if (TM_DEBUG > 1)
      tmPrint("EDGE (%d,%d) is put on Delaunay flip stack",
          e21->n1->index, e21->n2->index);
#endif
    }

    if (e22->is_local_delaunay == FALSE)
    {
      List_push(edges, e22);
#if (TM_DEBUG > 1)
      tmPrint("EDGE (%d,%d) is put on Delaunay flip stack",
          e22->n1->index, e22->n2->index);
#endif
    }

    tmPrint("EDGE-FLIP LIST COUNT: %d", edges->count);

  }

  List_destroy(edges);


#if (TM_DEBUG > 1)
  tmPrint("NUMBER OF DELAUNAY EDGE FLIPS: %d", n_flip);
#endif

error:
  return;


} /* tmMesh_delaunayFlip() */
