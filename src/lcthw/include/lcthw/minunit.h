#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <lcthw/dbg.h>
#include <stdlib.h>

int tests_run;

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) if (!(test)) {\
  log_err(message); return message; }

#define mu_run_test(test) debug("\n-----%s", " " #test); \
  message = test(); tests_run++; if (message) return message;

#define mu_print_tests_run() printf("TESTS RUN: %d\n", tests_run)

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
  argc = 0;argc++;\
  debug("----- RUNNING: %s", argv[0]);\
  printf("----\nRUNNING: %s\n", argv[0]);\
  char *result = name();\
  if (result != 0) {\
    printf("FAILED: %s\n", result);\
  }\
  else {\
    printf("ALL TESTS PASSED\n");\
  }\
  printf("Tests run: %d\n", tests_run);\
  exit(result != 0);\
}


#endif

//argc = 1; 
