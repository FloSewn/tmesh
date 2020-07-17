#include "tmesh/minunit.h"
#include "tmesh/tmList.h"
#include <assert.h>
#include "tmList_tests.h"

/*************************************************************
* Definition of unit test variables 
*************************************************************/
static tmList *list    = NULL;
static tmList *list_jn = NULL;
char *test1= "test1 data";
char *test2= "test2 data";
char *test3= "test3 data";
char *test4= "test4 data";

/*************************************************************
* Definition of unit test functions 
*************************************************************/
char *test_tmList_create()
{
  list = tmList_create();
  mu_assert(list != NULL, "Failed to create list.");

  list_jn = tmList_create();
  mu_assert(list_jn != NULL, "Failed to create list.");

  return NULL;
}

char *test_tmList_push_pop()
{
  tmList_push(list, test1);
  mu_assert(tmList_last(list) == test1, "Wrong last value.");

  tmList_push(list, test2);
  mu_assert(tmList_last(list) == test2, "Wrong last value.");

  tmList_push(list, test3);
  mu_assert(tmList_last(list) == test3, "Wrong last value.");
  mu_assert(tmList_count(list) == 3, "Wrong count on push.");

  char *val = tmList_pop(list);
  mu_assert(val == test3, "Wrong value on pop.");

  val = tmList_pop(list);
  mu_assert(val == test2, "Wrong value on pop.");

  val = tmList_pop(list);
  mu_assert(val == test1, "Wrong value on pop.");
  mu_assert(tmList_count(list) == 0, "Wrong count after pop.");

  return NULL;
}

char *test_tmList_unshift()
{
  tmList_unshift(list, test1);
  mu_assert(tmList_first(list) == test1, "Wrong first value.");

  tmList_unshift(list, test2);
  mu_assert(tmList_first(list) == test2, "Wrong first value.");

  tmList_unshift(list, test3);
  mu_assert(tmList_first(list) == test3, "Wrong first value.");
  mu_assert(tmList_count(list) == 3, "Wrong count on unshift.");

  return NULL;
}

char *test_tmList_remove()
{
  char *val = tmList_remove(list, list->first->next);
  mu_assert(val == test2, "Wrong removed element.");
  mu_assert(tmList_count(list) == 2, "Wrong count after remove.");
  mu_assert(tmList_first(list) == test3, "Wrong first after remove.");
  mu_assert(tmList_last(list) == test1, "Wrong last after remove.");

  return NULL;
}

char *test_tmList_shift()
{
  mu_assert(tmList_count(list) != 0, "Wrong count before shift.");

  char *val = tmList_shift(list);
  mu_assert(val == test3, "Wrong value on shift.");

  val = tmList_shift(list);
  mu_assert(val == test1, "Wrong value on shift.");
  mu_assert(tmList_count(list) == 0, "Wrong count after shift.");

  return NULL;
}

char *test_tmList_destroy()
{
  tmList_clear_destroy(list);

  return NULL;
}


char *test_tmList_join()
{
  tmList_push(list, test1);
  mu_assert(tmList_last(list) == test1, "Wrong last value.");

  tmList_push(list, test2);
  mu_assert(tmList_last(list) == test2, "Wrong last value.");

  tmList_push(list_jn, test3);
  mu_assert(tmList_last(list_jn) == test3, "Wrong last value.");

  tmList_push(list_jn, test4);
  mu_assert(tmList_last(list_jn) == test4, "Wrong last value.");

  tmList_join(list, list_jn);
  mu_assert(tmList_count(list) == 4, "Wrong join count.");
  mu_assert(tmList_first(list) == test1, "Wrong join first value.");
  mu_assert(list->first->next->next->value == test3, "Wrong join first value.");
  mu_assert(tmList_last(list) == test4, "Wrong join first value.");

  char *val;
  val = tmList_pop(list);
  mu_assert(val == test4, "Wrong value on pop.");

  val = tmList_pop(list);
  mu_assert(val == test3, "Wrong value on pop.");

  val = tmList_pop(list);
  mu_assert(val == test2, "Wrong value on pop.");

  val = tmList_pop(list);
  mu_assert(val == test1, "Wrong value on pop.");

  return NULL;

}


char *test_tmList_split()
{

  tmList *test_tmList = tmList_create();
  double test_data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};

  int i;
  for (i = 0; i < 5; i++)
    tmList_push(test_tmList, &test_data[i]);

  mu_assert( test_tmList->first->prev == NULL,
      "Wrong list definition.");
  mu_assert( test_tmList->last->next == NULL,
      "Wrong list definition.");

  tmList *split_tmList = tmList_split(test_tmList, 2);

  mu_assert( *(double*)(split_tmList->first->value) == 3.0,
      "Failed to split list.");
  mu_assert( *(double*)(split_tmList->last->value) == 5.0,
      "Failed to split list.");

  mu_assert( *(double*)(test_tmList->first->value) == 1.0,
      "Failed to split list.");
  mu_assert( *(double*)(test_tmList->last->value) == 2.0,
      "Failed to split list.");

  mu_assert( split_tmList->first->prev == NULL,
      "Failed to split list.");
  mu_assert( test_tmList->last->next == NULL,
      "Failed to split list.");
  mu_assert( test_tmList->count == 2,
      "Failed to split list.");
  mu_assert( split_tmList->count == 3,
      "Failed to split list.");

  tmList_destroy(split_tmList);

  split_tmList = tmList_split(test_tmList, 0);

  mu_assert( split_tmList == test_tmList,
      "Failed to split list.");

  split_tmList = tmList_split(test_tmList, test_tmList->count-1); 

  mu_assert( *(double*)(split_tmList->first->value) == 2.0,
      "Failed to split list.");
  mu_assert( *(double*)(split_tmList->last->value) == 2.0,
      "Failed to split list.");
  mu_assert( *(double*)(test_tmList->first->value) == 1.0,
      "Failed to split list.");
  mu_assert( *(double*)(test_tmList->last->value) == 1.0,
      "Failed to split list.");
  mu_assert( split_tmList->first->prev == NULL,
      "Failed to split list.");
  mu_assert( test_tmList->last->next == NULL,
      "Failed to split list.");

  mu_assert( test_tmList->count == 1,
      "Failed to split list.");
  mu_assert( split_tmList->count == 1,
      "Failed to split list.");

  tmList_destroy(split_tmList);


  split_tmList = tmList_split(test_tmList, 0);

  mu_assert( split_tmList == test_tmList,
      "Failed to split list.");
  

  tmList_destroy(test_tmList);

  return NULL;
}
