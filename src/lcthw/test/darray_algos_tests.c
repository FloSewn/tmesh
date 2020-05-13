#include <lcthw/minunit.h>
#include <lcthw/darray_algos.h>



/*************************************************************
* Definition of static unit test functions 
*************************************************************/
static int testcmp(char **a, char **b)
{
  return strcmp(*a, *b);
}

static DArray *create_words()
{
  DArray *result = DArray_create(0, 5);
  char *words[] = {"asdfasfd", "werwar", "13234", "asdfasfd", "oioj" };
  int i = 0;

  for (i = 0; i < 5; i++) {
    DArray_push(result, words[i]);
  }

  return result;
}

static int is_sorted(DArray *array)
{
  int i = 0;

  for (i = 0; i < DArray_count(array) - 1; i++) {
    if (strcmp(DArray_get(array, i), DArray_get(array, i + 1)) > 0) {
      return 0;
    }
  }

  return 1;
}

static char *run_sort_test(int (*func) (DArray *, DArray_compare), const char *name)
{
  DArray *words = create_words();
  mu_assert(!is_sorted(words), "Words should start not sorted.");

  debug("\n--- Testing %s sorting algorithm", name);
  int rc = func(words, (DArray_compare) testcmp);
  mu_assert(rc == 0, "sort failed.");
  mu_assert(is_sorted(words), "didn't sort it.");

  DArray_destroy(words);

  return NULL;
}

/*************************************************************
* Definition of unit test functions 
*************************************************************/
char *test_darray_qsort()
{
  return run_sort_test(DArray_qsort, "qsort");
}

char *test_darray_heapsort()
{
  return run_sort_test(DArray_heapsort, "heapsort");
}


