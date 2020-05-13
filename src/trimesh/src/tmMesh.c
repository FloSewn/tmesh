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
tmMesh *tmMesh_create(tmDouble xy_min[2], tmDouble xy_max[2])
{
  tmMesh *mesh = (tmMesh*) calloc( 1, sizeof(tmMesh) );
  check_mem(mesh);

  mesh->nodes_head = NULL;
  mesh->nodes_stack = List_create();
  mesh->nodes_index = 0;
  mesh->nodes_qtree = tmQtree_create(mesh, TM_NODE);
  tmQtree_init(mesh->nodes_qtree, NULL, 0, xy_min, xy_max); 

  mesh->edges_head = NULL;
  mesh->edges_stack = List_create();
  mesh->edges_index = 0;
  mesh->edges_qtree = tmQtree_create(mesh, TM_EDGE);
  tmQtree_init(mesh->edges_qtree, NULL, 0, xy_min, xy_max);
  
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
tmIndex tmMesh_addNode(tmMesh *mesh, tmNode *node)
{
  tmIndex node_index = mesh->nodes_index;
  mesh->nodes_index += 1;
  List_push(mesh->nodes_stack, node);
  tmQtree_addObj(mesh->nodes_qtree, node);
  
  return node_index;

} /* tmMesh_addNode() */

/**********************************************************
* Function: tmMesh_addEdge()
*----------------------------------------------------------
* Function to add a tmEdge to a tmMesh
*----------------------------------------------------------
* @return: tmEdge index on the mesh's edge stack
**********************************************************/
tmIndex tmMesh_addEdge(tmMesh *mesh, tmEdge *edge)
{
  tmIndex edge_index = mesh->edges_index;
  mesh->edges_index += 1;
  List_push(mesh->edges_stack, edge);
  tmQtree_addObj(mesh->edges_qtree, edge);
  
  return edge_index;

} /* tmMesh_addEdge() */

/**********************************************************
* Function: tmMesh_addTri()
*----------------------------------------------------------
* Function to add a tmTri to a tmMesh
*----------------------------------------------------------
* @return: tmTri index on the mesh's tri stack
**********************************************************/
tmIndex tmMesh_addTri(tmMesh *mesh, tmTri *tri)
{
  tmIndex tri_index = mesh->tris_index;
  mesh->tris_index += 1;
  List_push(mesh->tris_stack, tri);
  tmQtree_addObj(mesh->tris_qtree, tri);
  
  return tri_index;

} /* tmMesh_addTri() */


/**********************************************************
* Function: tmMesh_remNode()
*----------------------------------------------------------
* Function to remove a tmNode from a tmMesh
*----------------------------------------------------------
* @return: tmNode index on the mesh's node stack
*          returns -1 if node was not found
**********************************************************/
tmIndex tmMesh_remNode(tmMesh *mesh, tmNode *node)
{
  ListNode *cur, *nxt;
  tmBool    qtree_rem;

  tmIndex node_index = node->index;
  tmBool  in_stack   = FALSE;

  /*-------------------------------------------------------
  | Check if object is in the nodestack
  -------------------------------------------------------*/
  for (cur = mesh->nodes_stack->first; 
       cur != NULL; cur = cur->next)
  {
    if (cur->value == node)
    {
      in_stack = TRUE;
      break;
    }
  }

  if ( in_stack == FALSE )
  {
    log_warn("Can not remove node %d from mesh. Node not found.", node_index);
    return -1;
  }


  /*-------------------------------------------------------
  | Adjust indices of all following nodes
  -------------------------------------------------------*/
  nxt = cur->next;
  while (nxt != NULL)
  {
    ((tmNode*)nxt->value)->index -= 1;
    nxt = nxt->next;
  }

  /*-------------------------------------------------------
  | Remove node from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(mesh->nodes_qtree, node);
  mesh->nodes_index -= 1;

  /*-------------------------------------------------------
  | Remove node from stack
  -------------------------------------------------------*/
  List_remove(mesh->nodes_stack, cur);

  return node_index;

} /* tmMesh_addNode() */
