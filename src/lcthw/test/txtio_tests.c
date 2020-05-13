#include <lcthw/minunit.h>
#include <lcthw/txtio.h>
#include <lcthw/bstrlib.h>
#include "txtio_tests.h"

const char *testfile = "/home/florian/datadisk/Code/learn-C-the-hard-way/liblcthw2/src/lcthw/test/data/filio_test.txt";

/*************************************************************
* Unit test function to handle creation and 
* destruction of bstrings
*************************************************************/
char *test_txtio_readfile()
{
  int i;
  bstring *ptr  = NULL;
  Txtio   *file = Txtio_create( testfile );

  /*----------------------------------------------------------
  | Return all lines without '#' Comment specifier
  ----------------------------------------------------------*/
  struct bstrList *test_1 = Txtio_popLinesWith(file->txtlist, 
                                               "#");
  /*----------------------------------------------------------
  | Return all lines without '//' Comment specifier
  ----------------------------------------------------------*/
  struct bstrList *test_2 = Txtio_popLinesWith(test_1, "//");

  ptr            = test_2->entry;
  bstring bcmp_1 = bfromcstr( "comment" );

  for (i = 0; i < test_2->qty; i++)
    mu_assert( binstr(ptr[i], 0, bcmp_1) == BSTR_ERR,
      "<Txtio_popLinesWith> failed.");

  /*----------------------------------------------------------
  | Return all lines with 'Read this' Comment specifier
  ----------------------------------------------------------*/
  struct bstrList *test_3 = Txtio_getLinesWith(file->txtlist, 
                                               "Read this");

  ptr            = test_3->entry;
  bstring bcmp_2 = bfromcstr( "Read this" );

  for (i = 0; i < test_3->qty; i++)
    mu_assert(binstr(ptr[i], 0, bcmp_2) != BSTR_ERR, 
        "<Txtio_getLinesWith> failed.");

  /*----------------------------------------------------------
  | Return integer / float / bstring parameter
  ----------------------------------------------------------*/
  int int_param;
  double dbl_param;
  bstring bstr_param;

  int check = Txtio_intParam(file->txtlist, 
      "No existing parameter:", &int_param);
  mu_assert( check == 0, "<Txtio_intParam> failed.");

  check = Txtio_intParam(file->txtlist, 
       "Integer parameter:", &int_param);
  mu_assert( check == 1, "<Txtio_intParam> failed.");
  mu_assert( int_param == 4, "<Txtio_intParam> failed.");

  check = Txtio_extractParam(file->txtlist, 
       "Integer parameter:", 0, &int_param);
  mu_assert( check == 1, "<Txtio_extractParam> failed.");
  mu_assert( int_param == 4, "<Txtio_extractParam> failed.");

  check = Txtio_extractParam(file->txtlist, 
       "Read this:", 1, &dbl_param);
  mu_assert( check == 1, "<Txtio_extractParam> failed.");
  mu_assert( dbl_param == 1.2345, 
      "<Txtio_extractParam> failed.");

  check = Txtio_extractParam(file->txtlist, 
       "Read this parameter as string:", 2, &bstr_param);
  mu_assert( check == 1, "<Txtio_extractParam> failed.");
  mu_assert( strcmp( " key", bstr_param->data) == 0,
       "<Txtio_extractParam> failed.");

  bdestroy(bstr_param);
  Txtio_destroy( file );
  bdestroy( bcmp_1 );
  bdestroy( bcmp_2 );
  bstrListDestroy( test_1 );
  bstrListDestroy( test_2 );
  bstrListDestroy( test_3 );

  return NULL;
}
