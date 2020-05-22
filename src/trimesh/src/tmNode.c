#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmTri.h"
#include "trimesh/tmEdge.h"

/**********************************************************
* Function: tmNode_create()
*----------------------------------------------------------
* Create a new tmNode structure and returns a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new node
*
* @return: Pointer to a new tmNode structure
**********************************************************/
tmNode *tmNode_create(tmMesh *mesh, tmDouble xy[2])
{
  tmNode *node = (tmNode*) calloc( 1, sizeof(tmNode) );
  check_mem(node);

  /*-------------------------------------------------------
  | Add node to stack of mesh
  -------------------------------------------------------*/
  node->mesh  = mesh;
  node->xy[0] = xy[0];
  node->xy[1] = xy[1];

  /*-------------------------------------------------------
  | Init node properties
  -------------------------------------------------------*/
  node->is_active = FALSE;
  node->on_bdry   = FALSE;
  node->bdry_succ = NULL;
  node->bdry_prev = NULL;

  /*-------------------------------------------------------
  | Init node lists
  -------------------------------------------------------*/
  node->edges = List_create();
  node->n_edges = 0;

  node->tris = List_create();
  node->n_tris = 0;

  /*-------------------------------------------------------
  | Add node to qtree
  -------------------------------------------------------*/
  node->stack_pos = tmMesh_addNode(mesh, node);

  return node;
error:
  return NULL;
  
} /* tmNode_create() */

/**********************************************************
* Function: tmNode_destroy()
*----------------------------------------------------------
* Destroys a tmNode structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmNode to destroy
**********************************************************/ 
void tmNode_destroy(tmNode *node)
{
  /*-------------------------------------------------------
  | Remove node from the mesh structure 
  -------------------------------------------------------*/
  tmMesh_remNode(node->mesh, node);

  /*-------------------------------------------------------
  | Free all list structures
  -------------------------------------------------------*/
  List_destroy(node->edges);
  List_destroy(node->tris);

  /*-------------------------------------------------------
  | Finally free node structure memory
  -------------------------------------------------------*/
  free(node);

} /* tmNode_destroy() */
