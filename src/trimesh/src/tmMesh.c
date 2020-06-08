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
  | Mesh triangles 
  -------------------------------------------------------*/
  mesh->tris_head         = NULL;
  mesh->tris_stack        = List_create();
  mesh->no_tris           = 0;
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
  tmPrint(" MESH BOUNDARIES FREE");

  /*-------------------------------------------------------
  | Free advancing front structure
  -------------------------------------------------------*/
  tmFront_destroy(mesh->front);
  tmPrint(" MESH FRONT FREE");

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
  tmPrint(" MESH TRIS DESTROYED");

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
  tmPrint(" MESH NODES DESTROYED");

  /*-------------------------------------------------------
  | Free all quadtree structures
  -------------------------------------------------------*/
  tmQtree_destroy(mesh->nodes_qtree);
  tmPrint(" MESH NODES_QTREE FREE");
  tmQtree_destroy(mesh->tris_qtree);
  tmPrint(" MESH TRIS_QTREE FREE");

  /*-------------------------------------------------------
  | Free all list structures
  -------------------------------------------------------*/
  List_destroy(mesh->nodes_stack);
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
      is_inside &= !(tmBdry_isRight(cur_bdry, 
                                    obj, obj_type));
    }
    else
    {
      is_inside &= tmBdry_isLeftOn(cur_bdry, 
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
      ((tmEdge*)cur->value)->index = edge_index;
      fprintf(stdout,"%d\t%9d\t%9d\n", edge_index, ind1, ind2);
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
    
    tmPrint("-----------------------------------");
    tmPrint("FRONT-EDGE (%d/%d): (%d -> %d)", 
        n, front->no_edges,
        curEdge->n1->index, curEdge->n2->index);

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
