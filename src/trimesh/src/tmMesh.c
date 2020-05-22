#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
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
tmMesh *tmMesh_create(tmDouble xy_min[2], 
                      tmDouble xy_max[2], 
                      int      qtree_max_obj)
{
  tmMesh *mesh = (tmMesh*) calloc( 1, sizeof(tmMesh) );
  check_mem(mesh);

  /*-------------------------------------------------------
  | Mesh properties 
  -------------------------------------------------------*/
  mesh->qtree_max_obj = qtree_max_obj;

  /*-------------------------------------------------------
  | Mesh nodes 
  -------------------------------------------------------*/
  mesh->nodes_head = NULL;
  mesh->nodes_stack = List_create();
  mesh->nodes_index = 0;
  mesh->nodes_qtree = tmQtree_create(mesh, TM_NODE);
  tmQtree_init(mesh->nodes_qtree, NULL, 0, xy_min, xy_max); 

  /*-------------------------------------------------------
  | Mesh edges 
  -------------------------------------------------------*/
  mesh->edges_head = NULL;
  mesh->edges_stack = List_create();
  mesh->edges_index = 0;
  mesh->edges_qtree = tmQtree_create(mesh, TM_EDGE);
  tmQtree_init(mesh->edges_qtree, NULL, 0, xy_min, xy_max);
  
  /*-------------------------------------------------------
  | Mesh triangles 
  -------------------------------------------------------*/
  mesh->tris_head = NULL;
  mesh->tris_stack = List_create();
  mesh->tris_index = 0;
  mesh->tris_qtree = tmQtree_create(mesh, TM_TRI);
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
  | Free all nodes on the stack
  -------------------------------------------------------*/
  cur = nxt = mesh->nodes_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmNode_destroy(cur->value);
    cur = nxt;
  }

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

  /*-------------------------------------------------------
  | Free all quadtree structures
  -------------------------------------------------------*/
  tmQtree_destroy(mesh->nodes_qtree);
  tmQtree_destroy(mesh->edges_qtree);
  tmQtree_destroy(mesh->tris_qtree);

  /*-------------------------------------------------------
  | Free all list structures
  -------------------------------------------------------*/
  List_destroy(mesh->nodes_stack);
  List_destroy(mesh->edges_stack);
  List_destroy(mesh->tris_stack);

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
  mesh->nodes_index += 1;
  List_push(mesh->nodes_stack, node);
  tmQtree_addObj(mesh->nodes_qtree, node);

  ListNode *node_pos = List_last_node(mesh->nodes_stack);
  
  return node_pos;

} /* tmMesh_addNode() */

/**********************************************************
* Function: tmMesh_addEdge()
*----------------------------------------------------------
* Function to add a tmEdge to a tmMesh
*----------------------------------------------------------
* @return: tmEdge index on the mesh's edge stack
**********************************************************/
ListNode *tmMesh_addEdge(tmMesh *mesh, tmEdge *edge)
{
  mesh->edges_index += 1;
  List_push(mesh->edges_stack, edge);
  tmQtree_addObj(mesh->edges_qtree, edge);

  ListNode *edge_pos = List_last_node(mesh->edges_stack);
  
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
  mesh->tris_index += 1;
  List_push(mesh->tris_stack, tri);
  tmQtree_addObj(mesh->tris_qtree, tri);

  ListNode *tri_pos = List_last_node(mesh->tris_stack);
  
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
  ListNode *cur;
  tmBool    qtree_rem;

  if ( node->mesh != mesh )
  {
    log_warn("Can not remove node from mesh. Node not found.");
    return -1;
  }

  /*-------------------------------------------------------
  | Remove node from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(mesh->nodes_qtree, node);
  mesh->nodes_index -= 1;

  /*-------------------------------------------------------
  | Remove node from stack
  -------------------------------------------------------*/
  List_remove(mesh->nodes_stack, node->stack_pos);

} /* tmMesh_remNode() */

/**********************************************************
* Function: tmMesh_remEdge()
*----------------------------------------------------------
* Function to remove a tmEdge from a tmMesh
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
  {
    log_warn("Can not remove edge from mesh. Edge not found.");
    return -1;
  }

  /*-------------------------------------------------------
  | Remove node from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(mesh->edges_qtree, edge);
  mesh->edges_index -= 1;

  /*-------------------------------------------------------
  | Remove node from stack
  -------------------------------------------------------*/
  List_remove(mesh->edges_stack, edge->stack_pos);

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
  {
    log_warn("Can not remove tri from mesh. Tri not found.");
    return -1;
  }

  /*-------------------------------------------------------
  | Remove node from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(mesh->tris_qtree, tri);
  mesh->tris_index -= 1;

  /*-------------------------------------------------------
  | Remove node from stack
  -------------------------------------------------------*/
  List_remove(mesh->tris_stack, tri->stack_pos);

} /* tmMesh_remTri() */
