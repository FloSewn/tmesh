#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmBdry.h"
#include "trimesh/tmTri.h"


/**********************************************************
* Function: tmEdge_create()
*----------------------------------------------------------
* Create a new tmEdge structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new edge
* @param n1,n2: Start and ending nodes of this edge
*
* @return: Pointer to a new tmEdge structure
**********************************************************/
tmEdge *tmEdge_create(tmMesh *mesh, tmNode *n1, tmNode *n2)
{
  tmEdge *edge = (tmEdge*) calloc( 1, sizeof(tmEdge) );
  check_mem(edge);

  /*-------------------------------------------------------
  | Init nodes 
  -------------------------------------------------------*/
  edge->n1 = n1;
  edge->n2 = n2;

  /*-------------------------------------------------------
  | Positions of edge in nodes edge lists
  -------------------------------------------------------*/
  edge->n1_pos = NULL;
  edge->n2_pos = NULL;

  /*-------------------------------------------------------
  | Add edge to stack of mesh
  -------------------------------------------------------*/
  edge->mesh  = mesh;
  edge->bdry  = NULL;

  /*-------------------------------------------------------
  | Init edge properties
  -------------------------------------------------------*/
  edge->is_active   = FALSE;
  edge->is_on_front = FALSE;
  edge->is_on_bdry  = FALSE;
  edge->bdry_index  = -1;

  edge->xy[0] = 0.0;
  edge->xy[1] = 0.0;
  edge->len   = 0.0;
  edge->dxy_t[0] = 0.0;
  edge->dxy_t[1] = 0.0;
  edge->dxy_n[0] = 0.0;
  edge->dxy_n[1] = 0.0;

  /*-------------------------------------------------------
  | Init triangles 
  -------------------------------------------------------*/
  edge->t1 = NULL;
  edge->t2 = NULL;

  /*-------------------------------------------------------
  | Add edge to qtree
  -------------------------------------------------------*/
  edge->stack_pos = tmMesh_addEdge(mesh, edge);
  
  return edge;
error:
  return NULL;


} /* tmEdge_create() */

/**********************************************************
* Function: tmEdge_destroy()
*----------------------------------------------------------
* Destroys a tmEdge structure and frees all its memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmEdge to destroy
**********************************************************/
void tmEdge_destroy(tmEdge *edge)
{
  /*-------------------------------------------------------
  | Finally free edge structure memory
  -------------------------------------------------------*/
  free(edge);

} /* tmEdge_destroy() */


/**********************************************************
* Function: tmEdge_init()
*----------------------------------------------------------
* Init a new tmEdge structure 
*----------------------------------------------------------
* @param edge: tmEdge structure to init
*
**********************************************************/
void tmEdge_init(tmEdge *edge, tmBdry *bdry )
{
  /*-------------------------------------------------------
  | Init geometric edge properties
  -------------------------------------------------------*/
  edge->xy[0] = 0.5 * (edge->n1->xy[0] + edge->n2->xy[0]);
  edge->xy[1] = 0.5 * (edge->n1->xy[1] + edge->n2->xy[1]);

  const tmDouble dx = edge->n2->xy[0] - edge->n1->xy[0];
  const tmDouble dy = edge->n2->xy[1] - edge->n1->xy[1];

  edge->len = sqrt(dx*dx + dy*dy);

  edge->dxy_t[0] = dx / edge->len;
  edge->dxy_t[1] = dy / edge->len;

  edge->dxy_n[0] =-dy / edge->len;
  edge->dxy_n[1] = dx / edge->len;

  /*-------------------------------------------------------
  | Init boundary edge
  -------------------------------------------------------*/
  if ( bdry != NULL ) 
  {
    edge->bdry = bdry;

    List_push(edge->n1->bdry_edges, edge);
    edge->n1_pos           = List_last_node(edge->n1->bdry_edges);
    edge->n1->on_bdry      = TRUE;
    edge->n1->bdry_index   = bdry->bdry_index;
    edge->n1->n_bdry_edges += 1;

    List_push(edge->n2->bdry_edges, edge);
    edge->n2_pos           = List_last_node(edge->n2->bdry_edges);
    edge->n2->on_bdry      = TRUE;
    edge->n2->bdry_index   = bdry->bdry_index;
    edge->n2->n_bdry_edges += 1;
  }
  /*-------------------------------------------------------
  | Init front edge
  -------------------------------------------------------*/
  else 
  {
    List_push(edge->n1->front_edges, edge);
    edge->n1_pos            = List_last_node(edge->n1->front_edges);
    edge->n1->is_active     = TRUE;
    edge->n1->n_front_edges += 1;

    List_push(edge->n2->front_edges, edge);
    edge->n2_pos             = List_last_node(edge->n2->front_edges);
    edge->n2->is_active      = TRUE;
    edge->n2->n_front_edges += 1;
  }


} /* tmEdge_init()*/
