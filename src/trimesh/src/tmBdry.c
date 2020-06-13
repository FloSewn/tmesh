#include "trimesh/tmTypedefs.h"
#include "trimesh/tmMesh.h"
#include "trimesh/tmEdge.h"
#include "trimesh/tmNode.h"
#include "trimesh/tmBdry.h"
#include "trimesh/tmQtree.h"
#include "trimesh/tmFront.h"


/**********************************************************
* Function: tmBdry_create()
*----------------------------------------------------------
* Create a new tmBdry structure and returns a pointer
* to it.
*----------------------------------------------------------
* @return: Pointer to a new tmBdry structure
**********************************************************/
tmBdry *tmBdry_create(tmMesh *mesh, 
                      tmBool  is_interior,
                      int     index)
{
  tmBdry *bdry = (tmBdry*) calloc( 1, sizeof(tmBdry) );
  check_mem(bdry);

  /*-------------------------------------------------------
  | Properties of this boundary
  -------------------------------------------------------*/
  bdry->mesh        = mesh;
  bdry->no_edges    =  0;
  bdry->is_interior = is_interior;
  bdry->index       = index;
  bdry->mesh_pos    = NULL;

  /*-------------------------------------------------------
  | Boundary edges
  -------------------------------------------------------*/
  bdry->edges_head   = NULL;
  bdry->edges_stack  = List_create();
  bdry->edges_qtree  = tmQtree_create(mesh, TM_EDGE);
  tmQtree_init(bdry->edges_qtree, NULL, 0, 
               mesh->xy_min, mesh->xy_max);

  return bdry;

error:
  return NULL;

} /* tmBdry_create() */

/**********************************************************
* Function: tmBdry_destroy()
*----------------------------------------------------------
* Destroys a tmBdry structure and frees all its
* memory.
*----------------------------------------------------------
* @param *mesh: pointer to a tmBdry to destroy
*
**********************************************************/
void tmBdry_destroy(tmBdry *bdry)
{
  ListNode *cur, *nxt;

  /*-------------------------------------------------------
  | Free all boundary edges on the stack
  -------------------------------------------------------*/
  cur = nxt = bdry->edges_stack->first;
  while (nxt != NULL)
  {
    nxt = cur->next;
    tmBdry_remEdge(bdry, cur->value);
    cur = nxt;
  }

  /*-------------------------------------------------------
  | Destroy edges qtree and edges stack
  -------------------------------------------------------*/
  tmQtree_destroy(bdry->edges_qtree);
  List_destroy(bdry->edges_stack);

  /*-------------------------------------------------------
  | Remove boundary from the mesh structure 
  -------------------------------------------------------*/
  tmMesh_remBdry(bdry->mesh, bdry);

  /*-------------------------------------------------------
  | Finally free the structure
  -------------------------------------------------------*/
  free(bdry);

} /* tmBdry_destroy() */

/**********************************************************
* Function: tmBdry_addEdge()
*----------------------------------------------------------
* Function to add an edge to a tmBdry structure
* This edge is the new head of the boundary structure
*----------------------------------------------------------
* 
**********************************************************/
tmEdge *tmBdry_addEdge(tmBdry *bdry, 
                       tmNode *n1, 
                       tmNode *n2)
{
  tmEdge *edge = tmEdge_create( bdry->mesh, n1, n2, bdry );

  bdry->edges_head  = edge;
  bdry->no_edges += 1;

  List_push(bdry->edges_stack, edge);
  tmQtree_addObj(bdry->edges_qtree, edge);
  edge->stack_pos = List_last_node(bdry->edges_stack);
  
  return edge;

} /* tmBdry_addEdge() */

/**********************************************************
* Function: tmBdry_remEdge()
*----------------------------------------------------------
* Function to remove an edge from a tmBdry structure
*----------------------------------------------------------
*
**********************************************************/
void tmBdry_remEdge(tmBdry *bdry, tmEdge *edge)
{
  tmBool    qtree_rem;

  /*-------------------------------------------------------
  | Check if object is in this boundary
  -------------------------------------------------------*/
  if ( edge->bdry != bdry )
    log_warn("Can not remove edge from mesh. Edge not found.");

  /*-------------------------------------------------------
  | Remove edge from qtree
  -------------------------------------------------------*/
  qtree_rem = tmQtree_remObj(bdry->edges_qtree, edge);
  bdry->no_edges -= 1;

  /*-------------------------------------------------------
  | Remove edge from stack
  -------------------------------------------------------*/
  List_remove(bdry->edges_stack, edge->stack_pos);

  /*-------------------------------------------------------
  | Destroy edge
  -------------------------------------------------------*/
  tmEdge_destroy(edge);

} /* tmBdry_remEdge() */


/**********************************************************
* Function: tmBdry_isLeft()
*----------------------------------------------------------
* Check if an object is left of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isLeft(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_left = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_left &= tmEdge_isLeft( cur->value, obj, obj_type);
  }

  return is_left;

} /* tmBdry_isLeft() */

/**********************************************************
* Function: tmBdry_isLeftOn()
*----------------------------------------------------------
* Check if an object is left of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isLeftOn(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_left = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_left &= tmEdge_isLeftOn( cur->value, obj, obj_type);
  }

  return is_left;

} /* tmBdry_isLeftOn() */

/**********************************************************
* Function: tmBdry_isRight()
*----------------------------------------------------------
* Check if an object is right of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isRight(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_right = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_right &= tmEdge_isRight( cur->value, obj, obj_type);
  }

  return is_right;

} /* tmBdry_isRight() */

/**********************************************************
* Function: tmBdry_isRightOn()
*----------------------------------------------------------
* Check if an object is right of all boundary edges
*----------------------------------------------------------
* @param *edge: pointer to a tmEdge 
* @param *obj:  pointer to object to check for
* @param  obj_type: object type specifier
* @return boolean if object is located on the left of bdry
**********************************************************/
tmBool tmBdry_isRightOn(tmBdry *bdry, void *obj, int obj_type)
{
  ListNode *cur;
  tmBool is_right = TRUE;

  for (cur = bdry->edges_stack->first; 
       cur != NULL; cur = cur->next)
  {
    is_right &= tmEdge_isRightOn( cur->value, obj, obj_type);
  }

  return is_right;

} /* tmBdry_isRightOn() */

/**********************************************************
* Function: tmBdry_splitEdge()
*----------------------------------------------------------
* Split an edge by inserting a new node on its centroid
* and splitting it into two new edges
* This edge will be pointing from its node n1 to the new
* node and a new edge will be created, which points
* from the new node to n2.
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @param *edge: pointer to a tmEdge 
* @return: pointer to newly created first edge
**********************************************************/
tmEdge *tmBdry_splitEdge(tmBdry *bdry, tmEdge *edge)
{
  tmSizeFun sizeFun = bdry->mesh->sizeFun;

  tmNode *n1 = edge->n1;
  tmNode *n2 = edge->n2;

  /*-------------------------------------------------------
  | Move new node according to size function
  -------------------------------------------------------*/
  tmDouble rho_1 = sizeFun( edge->n1->xy );
  tmDouble rho_m = sizeFun( edge->xy );
  tmDouble *xy_t = edge->dxy_t;
  tmDouble *xy_1 = edge->n1->xy;

  tmDouble ln      =  0.5 * (rho_1 + rho_m); 
  tmDouble xy_n[2] = { xy_1[0] + ln * xy_t[0],
                       xy_1[1] + ln * xy_t[1] };

  /*-------------------------------------------------------
  | Create new node and edges
  -------------------------------------------------------*/
  tmNode *nn = tmNode_create(edge->mesh, xy_n);

  tmBdry_remEdge(bdry, edge);

  tmEdge *ne1 = tmBdry_addEdge(bdry, n1, nn);
  tmEdge *ne2 = tmBdry_addEdge(bdry, nn, n2);

  return ne1;

} /* tmEdge_split() */

/**********************************************************
* Function: tmBdry_refine()
*----------------------------------------------------------
* Refine the edges of a boundary structure according to
* a size function
*----------------------------------------------------------
* @param *bdry: pointer to bdry
* @return: 
**********************************************************/
void tmBdry_refine(tmBdry *bdry)
{

  ListNode *cur, *nxt;
  int counter = 0;

  cur = nxt = bdry->edges_stack->first;

  tmSizeFun sizeFun = bdry->mesh->sizeFun;

  while ( counter < bdry->no_edges )
  {
    nxt = cur->next;

    tmDouble rho_1 = sizeFun( ((tmEdge*)cur->value)->n1->xy );
    tmDouble rho_m = sizeFun( ((tmEdge*)cur->value)->xy );
    tmDouble rho   = rho_1 + rho_m;
    check( rho > TM_MIN_SIZE,
        "Size function return value lower than defined minimum scale.");

    if ( ((tmEdge*)cur->value)->len > rho )
      tmBdry_splitEdge(bdry, cur->value);
    else
      counter += 1;

    cur = nxt;

    if (cur == NULL)
      cur = bdry->edges_stack->first;
  }

error:
  return;

} /* tmBdry_refine() */
