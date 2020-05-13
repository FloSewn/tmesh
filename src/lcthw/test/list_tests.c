#include "lcthw/minunit.h"
#include "lcthw/list.h"
#include <assert.h>
#include "list_tests.h"

/*************************************************************
* Definition of unit test variables 
*************************************************************/
static List *list    = NULL;
static List *list_jn = NULL;
char *test1= "test1 data";
char *test2= "test2 data";
char *test3= "test3 data";
char *test4= "test4 data";

/*************************************************************
* Definition of unit test functions 
*************************************************************/
char *test_list_create()
{
  list = List_create();
  mu_assert(list != NULL, "Failed to create list.");

  list_jn = List_create();
  mu_assert(list_jn != NULL, "Failed to create list.");

  return NULL;
}

char *test_list_push_pop()
{
  List_push(list, test1);
  mu_assert(List_last(list) == test1, "Wrong last value.");

  List_push(list, test2);
  mu_assert(List_last(list) == test2, "Wrong last value.");

  List_push(list, test3);
  mu_assert(List_last(list) == test3, "Wrong last value.");
  mu_assert(List_count(list) == 3, "Wrong count on push.");

  char *val = List_pop(list);
  mu_assert(val == test3, "Wrong value on pop.");

  val = List_pop(list);
  mu_assert(val == test2, "Wrong value on pop.");

  val = List_pop(list);
  mu_assert(val == test1, "Wrong value on pop.");
  mu_assert(List_count(list) == 0, "Wrong count after pop.");

  return NULL;
}

char *test_list_unshift()
{
  List_unshift(list, test1);
  mu_assert(List_first(list) == test1, "Wrong first value.");

  List_unshift(list, test2);
  mu_assert(List_first(list) == test2, "Wrong first value.");

  List_unshift(list, test3);
  mu_assert(List_first(list) == test3, "Wrong first value.");
  mu_assert(List_count(list) == 3, "Wrong count on unshift.");

  return NULL;
}

char *test_list_remove()
{
  char *val = List_remove(list, list->first->next);
  mu_assert(val == test2, "Wrong removed element.");
  mu_assert(List_count(list) == 2, "Wrong count after remove.");
  mu_assert(List_first(list) == test3, "Wrong first after remove.");
  mu_assert(List_last(list) == test1, "Wrong last after remove.");

  return NULL;
}

char *test_list_shift()
{
  mu_assert(List_count(list) != 0, "Wrong count before shift.");

  char *val = List_shift(list);
  mu_assert(val == test3, "Wrong value on shift.");

  val = List_shift(list);
  mu_assert(val == test1, "Wrong value on shift.");
  mu_assert(List_count(list) == 0, "Wrong count after shift.");

  return NULL;
}

char *test_list_destroy()
{
  List_clear_destroy(list);
  List_clear_destroy(list_jn);

  return NULL;
}


char *test_list_join()
{
  List_push(list, test1);
  mu_assert(List_last(list) == test1, "Wrong last value.");

  List_push(list, test2);
  mu_assert(List_last(list) == test2, "Wrong last value.");

  List_push(list_jn, test3);
  mu_assert(List_last(list_jn) == test3, "Wrong last value.");

  List_push(list_jn, test4);
  mu_assert(List_last(list_jn) == test4, "Wrong last value.");

  List_join(list, list_jn);
  mu_assert(List_count(list) == 4, "Wrong join count.");
  mu_assert(List_first(list) == test1, "Wrong join first value.");
  mu_assert(list->first->next->next->value == test3, "Wrong join first value.");
  mu_assert(List_last(list) == test4, "Wrong join first value.");

  char *val;
  val = List_pop(list);
  mu_assert(val == test4, "Wrong value on pop.");

  val = List_pop(list);
  mu_assert(val == test3, "Wrong value on pop.");

  val = List_pop(list);
  mu_assert(val == test2, "Wrong value on pop.");

  val = List_pop(list);
  mu_assert(val == test1, "Wrong value on pop.");

  return NULL;

}

