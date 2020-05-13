#include <lcthw/minunit.h>
#include <lcthw/bstrlib.h>

#include "bstrlib_tests.h"

#define N 100


/*************************************************************
* Unit test function to handle creation and 
* destruction of bstrings
*************************************************************/
char *test_bstring_create_destroy()
{
  const char cstr[N] = "TestString";
  const char cstr2[N] = "NewString";

  /*----------------------------------------------------------
  | Create a bstring from a C-string
  ----------------------------------------------------------*/
  bstring bstr = bfromcstr( cstr );
  mu_assert(strcmp(cstr, bstr->data) == 0, 
      "<bfromcstr> failed.");

  /*----------------------------------------------------------
  | Create a bstring from any data-block and a given length
  ----------------------------------------------------------*/
  bstring bstr2 = blk2bstr(cstr2, 20);
  mu_assert(strcmp(cstr2, bstr2->data) == 0, 
      "<blk2bstr> failed.");

  /*----------------------------------------------------------
  | Copy a bstring
  ----------------------------------------------------------*/
  bstring bstr_cpy = bstrcpy(bstr);
  mu_assert(strcmp(cstr, bstr_cpy->data) == 0, 
      "<bstrcpy> failed.");

  /*----------------------------------------------------------
  | set one bstring to another
  | -> overwrite content of str A with content of str B
  ----------------------------------------------------------*/
  bassign(bstr_cpy, bstr2);
  mu_assert(strcmp(bstr_cpy->data, bstr2->data) == 0, 
      "<bassign> failed.");

  /*----------------------------------------------------------
  | set a bstring to c string's content
  ----------------------------------------------------------*/
  bassigncstr(bstr_cpy, cstr);
  mu_assert(strcmp(bstr_cpy->data, cstr) == 0, 
      "<bassigncstr> failed.");

  /*----------------------------------------------------------
  | Set a bstring to a C string but give the length
  ----------------------------------------------------------*/
  bassignblk(bstr_cpy, cstr2, 20);
  mu_assert(strcmp(bstr_cpy->data, cstr2) == 0, 
      "<bassignblk> failed.");

  /*----------------------------------------------------------
  | Set a bstring to a C string but give the length
  ----------------------------------------------------------*/

  bdestroy(bstr);
  bdestroy(bstr2);
  bdestroy(bstr_cpy);

  return NULL;
}


/*************************************************************
* Unit test function to handle concatenation of bstrings
*************************************************************/
char *test_bstring_concat()
{
  char str_A[N] = "Oh happy...";
  char str_B[N] = "DAY!";
  char str_AB[N] = "Oh happy...DAY!";

  bstring bstr_A = bfromcstr( str_A );
  bstring bstr_B = bfromcstr( str_B );

  /*----------------------------------------------------------
  | Append B to A
  ----------------------------------------------------------*/
  bconcat( bstr_A, bstr_B );
  mu_assert(strcmp(bstr_A->data, str_AB) == 0, 
      "<bconcat> failed.");

  bdestroy(bstr_A);
  bdestroy(bstr_B);

  return NULL;
}


/*************************************************************
* Unit test function to handle comparison of bstrings
*************************************************************/
char *test_bstring_compare()
{
  char str_A[N] = "Hi there\n";
  char str_B[N] = "there\n";
  char str_D[N] = "Corona\n";
  
  bstring bstr_A = bfromcstr( str_A );
  bstring bstr_B = bfromcstr( str_B );
  bstring bstr_C = bfromcstr( str_A );
  bstring bstr_D = bfromcstr( str_D );

  /*----------------------------------------------------------
  | Compare two bstrings
  ----------------------------------------------------------*/
  mu_assert(bstricmp(bstr_A, bstr_C) == 0, 
      "<bstricmp> failed.");

  /*----------------------------------------------------------
  | Test if two strings are equal
  | > biseq returns 1 for equal bstrings
  | > biseq returns 0 for different bstrings
  ----------------------------------------------------------*/
  mu_assert(biseq(bstr_A, bstr_C) == 1,
      "<biseq> failed.");
  mu_assert(biseq(bstr_A, bstr_B) == 0,
      "<biseq> failed.");

  /*----------------------------------------------------------
  | Test if bstr_B is in bstr_A, look from position 0
  | > If yes: return first position where it is found
  |         --> should return 3 for this example
  | > If no: return BSTR_ERR
  ----------------------------------------------------------*/
  mu_assert(binstr(bstr_A, 0, bstr_B) == 3,
      "<binstr> failed.");
  mu_assert(binstr(bstr_A, 0, bstr_D) == BSTR_ERR,
      "<binstr> failed.");

  /*----------------------------------------------------------
  | Find and replace 
  ----------------------------------------------------------*/
  char test_str[N] = "Hi Corona\n";
  bfindreplace(bstr_A, bstr_B, bstr_D, 0);
  mu_assert(strcmp(bstr_A->data, test_str) == 0, 
      "<bfindreplace> failed.");

  bdestroy(bstr_A);
  bdestroy(bstr_B);
  bdestroy(bstr_C);
  bdestroy(bstr_D);


  return NULL;
}


/*************************************************************
* Unit test function to handle bstring lists
*************************************************************/
char *test_bstring_list()
{
  char str_A[N] = "This is a test string.\n";

  char splitChar = ' ';

  bstring bstr_A = bfromcstr( str_A );

  struct bstrList *split_list = bsplit(bstr_A, splitChar);

  printf("Number of entries in bstrList: %d\n", split_list->qty);

  int i;
  bstring *ptr = split_list->entry;
  for (i = 0; i < split_list->qty; i++)
  {
    printf("Entry %d: %s\n", i, ptr[i]->data);
  }


  bstrListDestroy(split_list);

  bdestroy(bstr_A);

}
