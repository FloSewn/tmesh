#include "lcthw/minunit.h"
#include "lcthw/list.h"
#include "lcthw/list_algos.h"
#include <assert.h>
#include <string.h>

/***************************************************************
* Definition of unit test variables 
***************************************************************/
char *values[] = { "XXXX", "1234", "abcd", "xjvef", "NDSS" };

#define NUM_VALUES 5

/***************************************************************
* Definition of unit test functions 
***************************************************************/
List *test_list_algos_create_words()
{
  int i = 0;
  List *words = List_create();

  for (i = 0; i < NUM_VALUES; i++) {
    List_push(words, values[i]);
  }

  return words;
}

int test_list_algos_is_sorted(List *words)
{
  LIST_FOREACH(words, first, next, cur) {
    if (cur->next && strcmp(cur->value, cur->next->value) > 0 ) {
      debug("%s %s", (char *)cur->value,
          (char *)cur->next->value);
      return 0;
    }
  }
  
  return 1;
}

char *test_list_algos_bubble_sort()
{
  List *words = test_list_algos_create_words();

  // should work on a list that needs sorting
  int rc = List_bubble_sort(words, (List_compare) strcmp);
  mu_assert(rc == 0, "Bubble sort failed.");
  mu_assert(test_list_algos_is_sorted(words),
      "Words are not sorted after bubble sort.");

  // should work on a already sorted list
  rc = List_bubble_sort(words, (List_compare) strcmp);
  mu_assert(rc == 0, "Bubble sort of already sorted failed.");
  mu_assert(test_list_algos_is_sorted(words),
      "Words should be sorted of already sorted list.");

  List_destroy(words);

  // should work on an empty list
  words = List_create(words);
  rc = List_bubble_sort(words, (List_compare) strcmp);
  mu_assert(rc == 0, "Bubble sort of empty list failed.");
  mu_assert(test_list_algos_is_sorted(words),
      "Words should be sorted of empty list.");

  List_destroy(words);

  return NULL;
}

char *test_list_algos_merge_sort()
{
  List *words = test_list_algos_create_words();

  // should work on a list that needs sorting
  List *words_sort;
  words_sort = List_merge_sort(words, (List_compare) strcmp);
  mu_assert(words_sort, "Merge sort failed.");
  mu_assert(test_list_algos_is_sorted(words_sort),
      "Words are not sorted after merge sort.");
  List_destroy(words_sort);

  // should work on a already sorted list
  words_sort = List_merge_sort(words, (List_compare) strcmp);
  mu_assert(words_sort, "Merge sort of already sorted failed.");
  mu_assert(test_list_algos_is_sorted(words_sort),
      "Words should be sorted of already sorted list.");
  List_destroy(words_sort);
  List_destroy(words);

  // should work on an empty list
  words = List_create(words);
  words_sort = List_merge_sort(words, (List_compare) strcmp);
  mu_assert(words_sort, "Merge sort of empty list failed.");
  mu_assert(words == words_sort, "Merge sort should return empty input list.");
  mu_assert(test_list_algos_is_sorted(words_sort),
      "Words should be sorted of empty list.");

  List_destroy(words);


  return NULL;
}
