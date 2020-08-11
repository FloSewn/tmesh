/*
 * This source file is part of the tmesh library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 *
 * Most code in this file has been used from Zed Shaws Book
 * "Learn C the hard way"
 */
#include <stdlib.h>
#include "tmesh/tmList.h"
#include "tmesh/dbg.h"


/**************************************************************
* This function creates a new list structure
**************************************************************/
tmList *tmList_create()
{
  return calloc(1, sizeof(tmList));
}

/**************************************************************
* This function frees the memory of all tmListNodes of a given 
* list structure
**************************************************************/
void tmList_destroy(tmList *list)
{
  TMLIST_FOREACH(list, first, next, cur) {
    if (cur->prev) {
      free(cur->prev);
    }
  }
  free(list->last);
  free(list);
}

/**************************************************************
* This function frees all memory of the data that is stored 
* in a given list structure
**************************************************************/
void tmList_clear(tmList *list)
{
  TMLIST_FOREACH(list, first, next, cur) {
    free(cur->value);
  }
}

/**************************************************************
* This function frees all memory of a list structure
**************************************************************/
void tmList_clear_destroy(tmList *list) 
{
  tmList_clear(list);
  tmList_destroy(list);
}

/**************************************************************
* Push a new list node to the end of a list structure
**************************************************************/
void tmList_push(tmList *list, void *value)
{
  tmListNode *node = calloc(1, sizeof(tmListNode));
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

/**************************************************************
* Remove the last node from a list structure
**************************************************************/
void *tmList_pop(tmList *list)
{
  tmListNode *node = list->last;
  return node != NULL ? tmList_remove(list, node) : NULL;
}

/**************************************************************
* Push a new list node to the beginning of a list structure
**************************************************************/
void tmList_unshift(tmList *list, void *value)
{
  tmListNode *node = calloc(1, sizeof(tmListNode));
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

/**************************************************************
* Remove the first node from a list structure
**************************************************************/
void *tmList_shift(tmList *list)
{
  tmListNode *node = list->first;
  return node != NULL ? tmList_remove(list, node) : NULL;
}

/**************************************************************
* Remove a specific node from a list structure
**************************************************************/
void *tmList_remove(tmList *list, tmListNode *node)
{
  void *result = NULL;

  check(list->first && list->last, "tmList is empty.");
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
    tmListNode *after = node->next;
    tmListNode *before = node->prev;

    after->prev = before;
    before->next = after;
  }
  
  list->count--;
  result = node->value;
  free(node);

error:
  return result;
}

/**************************************************************
* Appends <list2> to the end of <list1>
**************************************************************/
void *tmList_join(tmList *list1, tmList *list2)
{
  check(list1->first || list2->first, "Both lists are empty.");

  if (list1->first == NULL) {
    list1->first = list2->first;
    list1->last  = list2->last;
    list1->count = list2->count;

  }
  else {
    list1->last->next = list2->first;
    list2->first->prev = list1->last;
    list1->last = list2->last;
    list1->count += list2->count;

  }

  list2->count = 0;
  list2->first = NULL;
  list2->last  = NULL;

  tmList_destroy(list2);

error:
  return NULL;
}


/*************************************************************
* Splits a list <list1> at a specified index of the list
*************************************************************/
tmList *tmList_split(tmList *list1, int index)
{
  if (index == 0)
    return list1;

  if (list1->count <= 1)
    return list1;

  if (index >= list1->count || index < 0)
    return list1;

  tmListNode *node = list1->first;

  int i = 0;
  while (i < index)
  {
    i++;
    node = node->next;
  }

  tmList *list2  = tmList_create();
  list2->first = node;
  list2->last  = list1->last;

  list1->last  = node->prev;

  node->prev->next = NULL;
  node->prev       = NULL;

  list2->count = list1->count - i;
  list1->count = i;
  
  return list2;

}




/*************************************************************
* Function for swapping the values of two nodes in a 
* list structure
*************************************************************/
static inline void tmListNode_swap(tmListNode *a, tmListNode *b);

/*************************************************************
* Function for merging two lists 
*************************************************************/
static inline tmList *tmList_merge(tmList *left, tmList *right, 
                               tmList_compare cmp);



/*************************************************************
* Function for swapping the values of two nodes in a 
* list structure
*************************************************************/
static inline void tmListNode_swap(tmListNode *a, tmListNode *b)
{
  void *temp = a->value;
  a->value = b->value;
  b->value = temp;
}

/*************************************************************
* Function for merging two lists 
*************************************************************/
static inline tmList *tmList_merge(tmList *left, tmList *right, 
                               tmList_compare cmp)
{
  tmList *result = tmList_create();
  void *val = NULL;

  while (tmList_count(left) > 0 || tmList_count(right) > 0) {
    if (tmList_count(left) > 0 && tmList_count(right) > 0) {
      if (cmp(tmList_first(left), tmList_first(right)) <= 0) {
        val = tmList_shift(left);
      } 
      else {
        val = tmList_shift(right);
      }
      tmList_push(result, val);
    }
    else if (tmList_count(left) > 0) {
      val = tmList_shift(left);
      tmList_push(result, val);
    }
    else if (tmList_count(right) > 0) {
      val = tmList_shift(right);
      tmList_push(result, val);
    }
  }

  return result;

}

/*************************************************************
* Bubble sort algorithm
*************************************************************/
int tmList_bubble_sort(tmList *list, tmList_compare cmp)
{
  int sorted = 1;

  if (tmList_count(list) <= 1) {
    return 0; // Already sorted 
  }

  do {
    sorted = 1;
    TMLIST_FOREACH(list, first, next, cur) {
      if (cur->next) {
        if (cmp(cur->value, cur->next->value) > 0) {
          tmListNode_swap(cur, cur->next);
          sorted = 0;
        }
      }
    }
  } while (!sorted);

  return 0;
}


/*************************************************************
* Merge sort algorithm
*************************************************************/
tmList *tmList_merge_sort(tmList *list, tmList_compare cmp)
{
  tmList *result = NULL;

  if (tmList_count(list) <= 1) {
    return list;
  }

  tmList *left  = tmList_create();
  tmList *right = tmList_create();
  int middle = tmList_count(list) / 2;

  TMLIST_FOREACH(list, first, next, cur) {
    if (middle > 0) {
      tmList_push(left, cur->value);
    }
    else {
      tmList_push(right, cur->value);
    }

    middle--;
  }

  tmList *sort_left  = tmList_merge_sort(left, cmp);
  tmList *sort_right = tmList_merge_sort(right, cmp);

  if (sort_left != left)
    tmList_destroy(left);
  if (sort_right != right)
    tmList_destroy(right);

  result = tmList_merge(sort_left, sort_right, cmp);

  tmList_destroy(sort_left);
  tmList_destroy(sort_right);

  return result;

}
