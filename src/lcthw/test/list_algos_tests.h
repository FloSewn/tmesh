#ifndef list_algos_tests_h
#define list_algos_tests_h
#include "lcthw/list.h"

List *test_list_algos_create_words();
int test_list_algos_is_sorted(List *words);
char *test_list_algos_bubble_sort();
char *test_list_algos_merge_sort();

#endif
