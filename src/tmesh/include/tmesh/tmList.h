/*
 * This header file is part of the tmesh library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 *
 * Most code in this file has been used from Zed Shaws Book
 * "Learn C the hard way"
 */
#ifndef TMESH_TMLIST_H
#define TMESH_TMLIST_H


/**************************************************************
* Defines for the handling with list structures
**************************************************************/
/* Returns the count of a list */
#define tmList_count(A) ((A)->count)
/* Return the value of the first list entry */
#define tmList_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
/* Return the value of the last list entry */
#define tmList_last(A) ((A)->last != NULL ? (A)->last->value : NULL)
/* Return the first list entry */
#define tmList_first_node(A) ((A)->first != NULL ? (A)->first : NULL)
/* Return the last list entry */
#define tmList_last_node(A) ((A)->last != NULL ? (A)->last : NULL)
/* Perform a loop over a list structure */
#define TMLIST_FOREACH(L, S, M, V) tmListNode *_node = NULL;\
                                 tmListNode *V = NULL;\
for(V = _node = L->S; _node != NULL; V = _node = _node->M)



/**************************************************************
* ListNode Structure
**************************************************************/
struct tmListNode;
typedef struct tmListNode {
  struct tmListNode *prev;
  struct tmListNode *next;
  void *value;
} tmListNode;

/**************************************************************
* List Structure
**************************************************************/
typedef struct tmList {
  int count;
  tmListNode *first;
  tmListNode *last;
} tmList;

/*************************************************************
* Function pointer to compare function
*************************************************************/
typedef int (*tmList_compare) (const void *a, const void *b);



/**************************************************************
* This function creates a new list structure
**************************************************************/
tmList *tmList_create();

/**************************************************************
* This function frees the memory of all ListNodes of a given 
* list structure
**************************************************************/
void tmList_destroy(tmList *list);

/**************************************************************
* This function frees all memory of the data that is stored 
* in a given list structure
**************************************************************/
void tmList_clear(tmList *list);

/**************************************************************
* This function frees all memory of a list structure
**************************************************************/
void tmList_clear_destroy(tmList *list);

/**************************************************************
* Push a new list node to the end of a list structure
**************************************************************/
void tmList_push(tmList *list, void *value);

/**************************************************************
* Remove the last node from a list structure
**************************************************************/
void *tmList_pop(tmList *list);

/**************************************************************
* Push a new list node to the beginning of a list structure
**************************************************************/
void tmList_unshift(tmList *list, void *value);

/**************************************************************
* Remove the first node from a list structure
**************************************************************/
void *tmList_shift(tmList *list);

/**************************************************************
* Remove a specific node from a list structure
**************************************************************/
void *tmList_remove(tmList *list, tmListNode *node);

/**************************************************************
* Appends <list2> to the end of <list1> and frees 
* the <list2> structure.
**************************************************************/
void *tmList_join(tmList *list1, tmList *list2);

/*************************************************************
* Splits a list <list1> at a specified Listnode <node>
*************************************************************/
tmList *tmList_split(tmList *list1, int index);


/*************************************************************
* Bubble sort algorithm
*************************************************************/
int tmList_bubble_sort(tmList *list, tmList_compare cmp);

/*************************************************************
* Merge sort algorithm
*************************************************************/
tmList *tmList_merge_sort(tmList *list, tmList_compare cmp);

#endif /* TMESH_TMLIST_H */
