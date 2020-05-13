#include <stdlib.h>
#include "lcthw/list.h"
#include "lcthw/dbg.h"


/***************************************************************
* This function creates a new list structure
***************************************************************/
List *List_create()
{
  return calloc(1, sizeof(List));
}

/***************************************************************
* This function frees the memory of all ListNodes of a given 
* list structure
***************************************************************/
void List_destroy(List *list)
{
  LIST_FOREACH(list, first, next, cur) {
    if (cur->prev) {
      free(cur->prev);
    }
  }
  free(list->last);
  free(list);
}

/***************************************************************
* This function frees all memory of the data that is stored 
* in a given list structure
***************************************************************/
void List_clear(List *list)
{
  LIST_FOREACH(list, first, next, cur) {
    free(cur->value);
  }
}

/***************************************************************
* This function frees all memory of a list structure
***************************************************************/
void List_clear_destroy(List *list) 
{
  List_clear(list);
  List_destroy(list);
}

/***************************************************************
* Push a new list node to the end of a list structure
***************************************************************/
void List_push(List *list, void *value)
{
  ListNode *node = calloc(1, sizeof(ListNode));
  check_mem(node);

  node->value = value;

  if (list->last == NULL) {
    list->first = node;
    list->last = node;
  }
  else {
    list->last->next = node;
    node->prev = list->last;
    list->last = node;
  }
  
  list->count++;

error:
  return;
}

/***************************************************************
* Remove the last node from a list structure
***************************************************************/
void *List_pop(List *list)
{
  ListNode *node = list->last;
  return node != NULL ? List_remove(list, node) : NULL;
}

/***************************************************************
* Push a new list node to the beginning of a list structure
***************************************************************/
void List_unshift(List *list, void *value)
{
  ListNode *node = calloc(1, sizeof(ListNode));
  check_mem(node);

  node->value = value;

  if (list->first == NULL) {
    list->first = node;
    list->last = node;
  }
  else {
    node->next = list->first;
    list->first->prev = node;
    list->first = node;
  }
  
  list->count++;

error:
  return;
}

/***************************************************************
* Remove the first node from a list structure
***************************************************************/
void *List_shift(List *list)
{
  ListNode *node = list->first;
  return node != NULL ? List_remove(list, node) : NULL;
}

/***************************************************************
* Remove a specific node from a list structure
***************************************************************/
void *List_remove(List *list, ListNode *node)
{
  void *result = NULL;

  check(list->first && list->last, "List is empty.");
  check(node, "node can't be NULL.");

  if (node == list->first && node == list->last) {
    list->first = NULL;
    list->last = NULL;
  }
  else if (node == list->first) {
    list->first = node->next;
    check(list->first != NULL, "Invalid list, somehow got a first that is NULL.");
    list->first->prev = NULL;
  } 
  else if (node == list->last) {
    list->last = node->prev;
    check(list->last != NULL, "Invalid list, somehow got a last that is NULL.");
    list->last->next = NULL;
  }
  else {
    ListNode *after = node->next;
    ListNode *before = node->prev;

    after->prev = before;
    before->next = after;
  }
  
  list->count--;
  result = node->value;
  free(node);

error:
  return result;
}

/***************************************************************
* Appends <list2> to the end of <list1>
***************************************************************/
void *List_join(List *list1, List *list2)
{
  check(list1->first || list2->first, "Both lists are empty.");

  if (list1->first == NULL) {
    list1->first = list2->first;
    list1->count = list2->count;
  }
  else {
    list1->last->next = list2->first;
    list2->first->prev = list1->last;
    list1->last = list2->last;
    list1->count += list2->count;

    list2->count = 0;
    list2->first = NULL;
    list2->last  = NULL;
  }

error:
  return NULL;
}



