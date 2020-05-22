#ifndef lcthw_list_h
#define lcthw_list_h



/***************************************************************
* Defines for the handling with list structures
***************************************************************/
/* Returns the count of a list */
#define List_count(A) ((A)->count)
/* Return the value of the first list entry */
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
/* Return the value of the last list entry */
#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)
/* Return the first list entry */
#define List_first_node(A) ((A)->first != NULL ? (A)->first : NULL)
/* Return the last list entry */
#define List_last_node(A) ((A)->last != NULL ? (A)->last : NULL)
/* Perform a loop over a list structure */
#define LIST_FOREACH(L, S, M, V) ListNode *_node = NULL;\
                                 ListNode *V = NULL;\
for(V = _node = L->S; _node != NULL; V = _node = _node->M)



/**************************************************************
* ListNode Structure
**************************************************************/
struct ListNode;
typedef struct ListNode {
  struct ListNode *prev;
  struct ListNode *next;
  void *value;
} ListNode;

/**************************************************************
* List Structure
**************************************************************/
typedef struct List {
  int count;
  ListNode *first;
  ListNode *last;
} List;



/**************************************************************
* This function creates a new list structure
**************************************************************/
List *List_create();

/**************************************************************
* This function frees the memory of all ListNodes of a given 
* list structure
**************************************************************/
void List_destroy(List *list);

/**************************************************************
* This function frees all memory of the data that is stored 
* in a given list structure
**************************************************************/
void List_clear(List *list);

/**************************************************************
* This function frees all memory of a list structure
**************************************************************/
void List_clear_destroy(List *list);

/**************************************************************
* Push a new list node to the end of a list structure
**************************************************************/
void List_push(List *list, void *value);

/**************************************************************
* Remove the last node from a list structure
**************************************************************/
void *List_pop(List *list);

/**************************************************************
* Push a new list node to the beginning of a list structure
**************************************************************/
void List_unshift(List *list, void *value);

/**************************************************************
* Remove the first node from a list structure
**************************************************************/
void *List_shift(List *list);

/**************************************************************
* Remove a specific node from a list structure
**************************************************************/
void *List_remove(List *list, ListNode *node);

/**************************************************************
* Appends <list2> to the end of <list1> and frees 
* the <list2> structure.
**************************************************************/
void *List_join(List *list1, List *list2);

#endif /* lcthw_list_h */
