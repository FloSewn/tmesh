#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmBdry.h"
#include "trimesh/tmFront.h"
#include "trimesh/tmTri.h"


/**********************************************************
* Function: tmEdge_create()
*----------------------------------------------------------
* Create a new tmEdge structure and return a pointer
* to it.
*----------------------------------------------------------
* @param mesh: parent mesh of the new edge
* @param n1,n2: Start and ending nodes of this edge
* @param type: flag for boundary / front / mesh edge
*               boundary -> 0
*               front    -> 1
*               mesh     -> 2
*
* @return: Pointer to a new tmEdge structure
**********************************************************/
tmEdge *tmEdge_create(tmMesh *mesh, 
                      tmNode *n1, tmNode *n2,
                      tmBdry *bdry,
                      int     edgeType)
{
  tmEdge *edge = (tmEdge*) calloc( 1, sizeof(tmEdge) );
  check_mem(edge);

  /*-------------------------------------------------------
  | Init edge pointers
  -------------------------------------------------------*/
  edge->mesh  = mesh;
  edge->bdry  = NULL;
  edge->front = NULL;

  edge->qtree_pos = NULL;
  edge->stack_pos = NULL;

  edge->is_on_bdry        = FALSE;
  edge->is_on_front       = FALSE;
  edge->is_on_mesh        = FALSE;
  edge->is_local_delaunay = FALSE;
  edge->bdry_marker       = -1;

  /*-------------------------------------------------------
  | Init nodes 
  -------------------------------------------------------*/
  edge->n1 = n1;
  edge->n2 = n2;

  /*-------------------------------------------------------
  | Init triangles 
  -------------------------------------------------------*/
  edge->t1 = NULL;
  edge->t2 = NULL;

  /*-------------------------------------------------------
  | Init buffer variables ( e.g. for sorting )
  -------------------------------------------------------*/
  edge->dblBuf = 0.0;
  edge->indBuf = 0;

  /*-------------------------------------------------------
  | Init geometric edge properties
  -------------------------------------------------------*/
  edge->xy[0] = 0.5 * (n1->xy[0] + n2->xy[0]);
  edge->xy[1] = 0.5 * (n1->xy[1] + n2->xy[1]);

  const tmDouble dx = n2->xy[0] - n1->xy[0];
  const tmDouble dy = n2->xy[1] - n1->xy[1];

  edge->len = sqrt(dx*dx + dy*dy);

  edge->dxy_t[0] = dx / edge->len;
  edge->dxy_t[1] = dy / edge->len;

  edge->dxy_n[0] =-dy / edge->len;
  edge->dxy_n[1] = dx / edge->len;


  /*-------------------------------------------------------
  | Specifiy boundary edge properties
  -------------------------------------------------------*/
  if ( edgeType == 0 )
  {
    edge->bdry  = bdry;
    edge->is_on_bdry  = TRUE;

    List_push(edge->n1->bdry_edges, edge);
    edge->n1_pos = List_last_node(edge->n1->bdry_edges);
    edge->n1->on_bdry = TRUE;
    edge->n1->n_bdry_edges += 1;

    List_push(edge->n2->bdry_edges, edge);
    edge->n2_pos = List_last_node(edge->n2->bdry_edges);
    edge->n2->on_bdry = TRUE;
    edge->n2->n_bdry_edges += 1;

    edge->stack_pos = tmBdry_addEdge(bdry, edge);
  }
  /*-------------------------------------------------------
  | Specifiy advancing front edge properties
  -------------------------------------------------------*/
  else if ( edgeType == 1)
  {
    edge->front = mesh->front;
    edge->is_on_front = TRUE;

    List_push(edge->n1->front_edges, edge);
    edge->n1_pos = List_last_node(edge->n1->front_edges);
    edge->n1->on_front = TRUE;
    edge->n1->n_front_edges += 1;

    List_push(edge->n2->front_edges, edge);
    edge->n2_pos = List_last_node(edge->n2->front_edges);
    edge->n2->on_front = TRUE;
    edge->n2->n_front_edges += 1;

    edge->stack_pos = tmFront_addEdge(mesh->front, edge);
  }
  /*-------------------------------------------------------
  | Specifiy mesh edge properties
  -------------------------------------------------------*/
  else if ( edgeType == 2)
  {
    edge->is_on_mesh = TRUE;

    List_push(edge->n1->mesh_edges, edge);
    edge->n1_pos = List_last_node(edge->n1->mesh_edges);
    edge->n1->n_mesh_edges += 1;

    List_push(edge->n2->mesh_edges, edge);
    edge->n2_pos = List_last_node(edge->n2->mesh_edges);
    edge->n2->n_mesh_edges += 1;

    edge->stack_pos = tmMesh_addEdge(mesh, edge);
  }
  else
  {
    log_err("Unknown edge type %d", edgeType);
  }

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
  | Handle boundary edge
  -------------------------------------------------------*/
  if ( edge->is_on_bdry == TRUE )
  {
    tmBdry_remEdge(edge->bdry, edge);

    /* Remove edge from node 1 edge list                 */
    List_remove(edge->n1->bdry_edges, edge->n1_pos);
    edge->n1->n_bdry_edges -= 1;
    if (edge->n1->n_bdry_edges <= 0)
      edge->n1->on_bdry = FALSE;

    /* Remove edge from node 2 edge list                 */
    List_remove(edge->n2->bdry_edges, edge->n2_pos);
    edge->n2->n_bdry_edges -= 1;
    if (edge->n2->n_bdry_edges <= 0)
      edge->n2->on_bdry = FALSE;
  }
  /*-------------------------------------------------------
  | Handle front edge
  -------------------------------------------------------*/
  else if ( edge->is_on_front == TRUE )
  {
    tmFront_remEdge(edge->front, edge);

    /* Remove edge from node 1 edge list                 */
    List_remove(edge->n1->front_edges, edge->n1_pos);
    edge->n1->n_front_edges -= 1;
    if (edge->n1->n_front_edges <= 0)
      edge->n1->on_front = FALSE;

    /* Remove edge from node 2 edge list                 */
    List_remove(edge->n2->front_edges, edge->n2_pos);
    edge->n2->n_front_edges -= 1;
    if (edge->n2->n_front_edges <= 0)
      edge->n2->on_front = FALSE;
  }
  /*-------------------------------------------------------
  | Handle mesh edge
  -------------------------------------------------------*/
  else if ( edge->is_on_mesh == TRUE )
  {
    tmMesh_remEdge(edge->mesh, edge);

    List_remove(edge->n1->mesh_edges, edge->n1_pos);
    edge->n1->n_mesh_edges -= 1;

    List_remove(edge->n2->mesh_edges, edge->n2_pos);
    edge->n2->n_mesh_edges -= 1;

  }
  else
    log_err("Removing edge of unknown type.");

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
void tmEdge_init(tmEdge *edge, tmBdry *bdry, tmFront *front)
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
    edge->n1->n_bdry_edges += 1;

    List_push(edge->n2->bdry_edges, edge);
    edge->n2_pos           = List_last_node(edge->n2->bdry_edges);
    edge->n2->on_bdry      = TRUE;
    edge->n2->n_bdry_edges += 1;
  }
  /*-------------------------------------------------------
  | Init front edge
  -------------------------------------------------------*/
  else 
  {
    List_push(edge->n1->front_edges, edge);
    edge->n1_pos            = List_last_node(edge->n1->front_edges);
    edge->n1->on_front      = TRUE;
    edge->n1->n_front_edges += 1;

    List_push(edge->n2->front_edges, edge);
    edge->n2_pos             = List_last_node(edge->n2->front_edges);
    edge->n2->on_front       = TRUE;
    edge->n2->n_front_edges += 1;
  }

} /* tmEdge_init()*/


/**********************************************************
* Function: tmEdge_isLeft()
*----------------------------------------------------------
* Check if an object is left of the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isLeft(tmEdge *edge, void *obj, int obj_type)
{
  tmDouble *xy;

  if ( obj_type == TM_NODE)
    xy = ((tmNode*)obj)->xy;
  else if ( obj_type == TM_EDGE)
    xy = ((tmEdge*)obj)->xy;
  else if ( obj_type == TM_TRI)
    xy = ((tmTri*)obj)->xy;
  else
    log_err("Wrong type provied for tmEdge_isLeft()");

  return IS_LEFT( edge->n1->xy, edge->n2->xy, xy);


} /* tmEdge_isLeft() */

/**********************************************************
* Function: tmEdge_isLeftOn()
*----------------------------------------------------------
* Check if an object is left of or on the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isLeftOn(tmEdge *edge, void *obj, int obj_type)
{
  tmDouble *xy;

  if ( obj_type == TM_NODE)
    xy = ((tmNode*)obj)->xy;
  else if ( obj_type == TM_EDGE)
    xy = ((tmEdge*)obj)->xy;
  else if ( obj_type == TM_TRI)
    xy = ((tmTri*)obj)->xy;
  else
    log_err("Wrong type provied for tmEdge_isLeftOn()");

  return IS_LEFTON( edge->n1->xy, edge->n2->xy, xy);

} /* tmEdge_isLeftOn() */


/**********************************************************
* Function: tmEdge_isRight()
*----------------------------------------------------------
* Check if an object is right of the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isRight(tmEdge *edge, void *obj, int obj_type)
{
  tmDouble *xy;

  if ( obj_type == TM_NODE)
    xy = ((tmNode*)obj)->xy;
  else if ( obj_type == TM_EDGE)
    xy = ((tmEdge*)obj)->xy;
  else if ( obj_type == TM_TRI)
    xy = ((tmTri*)obj)->xy;
  else
    log_err("Wrong type provied for tmEdge_isRight()");

  return IS_RIGHT( edge->n1->xy, edge->n2->xy, xy);


} /* tmEdge_isRight() */

/**********************************************************
* Function: tmEdge_isRightOn()
*----------------------------------------------------------
* Check if an object is right of or on the edge
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of edge
**********************************************************/
tmBool tmEdge_isRightOn(tmEdge *edge, void *obj, int obj_type)
{
  tmDouble *xy;

  if ( obj_type == TM_NODE)
    xy = ((tmNode*)obj)->xy;
  else if ( obj_type == TM_EDGE)
    xy = ((tmEdge*)obj)->xy;
  else if ( obj_type == TM_TRI)
    xy = ((tmTri*)obj)->xy;
  else
    log_err("Wrong type provied for tmEdge_isRightOn()");

  return IS_RIGHTON( edge->n1->xy, edge->n2->xy, xy);

} /* tmEdge_isRightOn() */

/**********************************************************
* Function: tmEdge_compareLen()
*----------------------------------------------------------
* Compare two edges according to their lengths
*
* -> Returns an integer less than zero, if length of e1 is 
*    less than lenght of e2
*
* -> Returns zero, if length of e1 equals length of e2
*
*  -> Returns an integer greater than zero if length of 
*     e1 is greater than length of e2
*----------------------------------------------------------
* @param *e1, *e2: pointer to tmEdges to compare
**********************************************************/
int tmEdge_compareLen(tmEdge *e1, tmEdge *e2)
{
  if (e1->len > e2->len)
    return 1;

  if EQ(e1->len, e2->len)
    return 0;

  return -1;

} /*tmEdge_compareLen() */

/**********************************************************
* Function: tmEdge_createNode()
*----------------------------------------------------------
* Create new node perpendicular to an edge with a distance
* according to a size function
*----------------------------------------------------------
* @param edge: pointer to edge
* 
**********************************************************/
tmNode *tmEdge_createNode(tmEdge *edge)
{
  tmDouble *xy_e  = edge->xy;
  tmDouble *dxy_n = edge->dxy_n;

  tmSizeFun sizeFun = edge->mesh->sizeFun;

  tmDouble fac    = TM_NEW_NODE_DIST_FAC;
  tmDouble d      = fac * sizeFun(xy_e);

  tmDouble xy_n[2]  = { xy_e[0] + d * dxy_n[0], 
                        xy_e[1] + d * dxy_n[1] };

  tmNode *n = tmNode_create(edge->mesh, xy_n);

  return n;

} /* tmEdge_createNode() */

/**********************************************************
* Function: tmEdge_isLocalDelaunay()
*----------------------------------------------------------
* Check if an edge is locally delaunay
*----------------------------------------------------------
* @param edge: pointer to edge
* 
**********************************************************/
tmBool tmEdge_isLocalDelaunay(tmEdge *edge)
{
  tmNode *nl, *nr;

  tmTri *tl = edge->t1;
  tmTri *tr = edge->t2;

  tmNode *n1 = edge->n1;
  tmNode *n2 = edge->n2;


  if (tl == NULL || tr == NULL)
    return TRUE;

  if ( n1 == tl->n1 )
  {
    nl = tl->n3;
    check( n2 == tl->n2, "Wrong triangle-node connectivity");
  }
  else if ( n1 == tl->n2 )
  {
    nl = tl->n1;
    check( n2 == tl->n3, "Wrong triangle-node connectivity");
  }
  else if ( n1 == tl->n3 )
  {
    nl = tl->n2;
    check( n2 == tl->n1, "Wrong triangle-node connectivity");
  }
  else
    log_err("Wrong triangle-edge connectivity");



  if ( n1 == tr->n1 )
  {
    nr = tr->n2;
    check( n2 == tr->n3, "Wrong triangle-node connectivity");
  }
  else if ( n1 == tr->n2 )
  {
    nr = tr->n3;
    check( n2 == tr->n1, "Wrong triangle-node connectivity");
  }
  else if ( n1 == tr->n3 )
  {
    nr = tr->n1;
    check( n2 == tr->n2, "Wrong triangle-node connectivity");
  }
  else
    log_err("Wrong triangle-edge connectivity");



  tmDouble dx_l = nl->xy[0] - tr->circ_xy[0];
  tmDouble dy_l = nl->xy[1] - tr->circ_xy[1];

  tmDouble dx_r = nr->xy[0] - tl->circ_xy[0];
  tmDouble dy_r = nr->xy[1] - tl->circ_xy[1];

  tmDouble r2_l = dx_l*dx_l + dy_l*dy_l;
  tmDouble r2_r = dx_r*dx_r + dy_r*dy_r;

  tmDouble circ2_l = tl->circ_r * tl->circ_r;
  tmDouble circ2_r = tr->circ_r * tr->circ_r;

  if (r2_l >= circ2_r && r2_r >= circ2_l)
  {
    return TRUE;
  }
#if (TM_DEBUG > 1)
    tmPrint("EDGE (%d,%d) IS NOT DELAUNAY -> (%.4f,%.4f|%.4f,%.4f) -> (%.4f,%.4f|%.4f,%.4f)",
        n1->index, n2->index, 
        tl->circ_xy[0], tl->circ_xy[1], circ2_l, r2_r,
        tr->circ_xy[0], tr->circ_xy[1], circ2_r, r2_l
        );
#endif

  return FALSE;

error:
  return TRUE;

} /* tmEdge_isLocalDelaunay() */
