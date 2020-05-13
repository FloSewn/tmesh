#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <lcthw/dbg.h>
#include <lcthw/txtio.h>
#include <lcthw/bstrlib.h>



/*************************************************************
* Function to create a new file reader
*
* Reference:
* https://stackoverflow.com/questions/14002954/c-programming
* -how-to-read-the-whole-file-contents-into-a-buffer
*************************************************************/
Txtio *Txtio_create(const char *file_path)
{
  /*---------------------------------------------------------
  | Allocate memory for txtio structure 
  ---------------------------------------------------------*/
  Txtio *txtfile = calloc(1, sizeof(Txtio));
  check_mem(txtfile);

  txtfile->path = file_path;

  /*---------------------------------------------------------
  | Open text file and copy its data 
  ---------------------------------------------------------*/
  FILE *fptr = NULL;
  fptr = fopen(txtfile->path, "rb");
  check(fptr, "Failed to open %s.", txtfile->path);

  /* Estimate length of chars in whole file                */
  fseek(fptr, 0, SEEK_END);
  long length = ftell(fptr);
  fseek(fptr, 0, SEEK_SET);

  /* Read total file into buffer                           */
  char *buffer = (char *) malloc(length + 1);
  buffer[length] = '\0';
  fread(buffer, 1, length, fptr);

  /* Copy relevant data to Txtio structure                 */
  bstring bbuffer = bfromcstr( buffer );
  txtfile->txt    = bbuffer;
  txtfile->length = length + 1;

  /* Split buffer according to newlines                    */
  char splitter = '\n';
  txtfile->txtlist = bsplit(bbuffer, splitter);
  txtfile->nlines = txtfile->txtlist->qty;

  fclose(fptr);
  free(buffer);

  return txtfile;
error:
  return NULL;
}


/*************************************************************
* Function to destroy a file reader structure
*************************************************************/
int Txtio_destroy(Txtio *file)
{
  bstrListDestroy(file->txtlist);
  bdestroy(file->txt);
  free(file);
  return 0;
}


/*************************************************************
* Function returns a bstring list of lines, that 
* do not contain a specifier
*************************************************************/
struct bstrList *Txtio_popLinesWith(struct bstrList *txtlist, 
                                    const char *fltr)
{
  bstring *fl_ptr = txtlist->entry;
  bstring  bfltr  = bfromcstr(fltr); 

  bstring *bl_ptr     = NULL;
  struct bstrList *bl = NULL;
  int i;
  int hits = 0;
  int cnt  = 0;
  int *marker = (int*) calloc(txtlist->qty, sizeof(int));
  check_mem(marker);

  /*----------------------------------------------------------
  | Fill array of markers with line numbers, that do not 
  | contain the filter string
  ----------------------------------------------------------*/
  for (i = 0; i < txtlist->qty; i++) 
  {
    if ( binstr(fl_ptr[i], 0, bfltr) == BSTR_ERR )
    {
      hits += 1;
      marker[i] = i;
    }
    else
    {
      marker[i] = -1;
    }
  }

  /*----------------------------------------------------------
  | Create new bstrList that will contained all filtered lines
  ----------------------------------------------------------*/
  bl = bstrListCreate();
  bstrListAlloc(bl, hits);

  /*----------------------------------------------------------
  | Copy marked lines into new bstrList
  ----------------------------------------------------------*/
  bl_ptr = bl->entry;

  for (i = 0; i < txtlist->qty; i++) 
  {
    if (marker[i] >= 0)
    {
      const int curline = marker[i];
      bl_ptr[cnt] = bstrcpy(fl_ptr[curline]);
      bl->qty += 1;
      cnt += 1;
    }
  }

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy(bfltr);
  free(marker);

  return bl;
error:

  bdestroy(bfltr);
  free(marker);
  return NULL;
}
  

/*************************************************************
* Function returns a bstring list of lines, that 
* do contain a certain specifier
*************************************************************/
struct bstrList *Txtio_getLinesWith(struct bstrList *txtlist,
                                    const char *fltr)
{
  bstring *fl_ptr = txtlist->entry;
  bstring  bfltr  = bfromcstr( fltr ); 

  bstring *bl_ptr     = NULL;
  struct bstrList *bl = NULL;

  int i;
  int hits = 0;
  int cnt  = 0;
  int *marker = (int*) calloc(txtlist->qty, sizeof(int));
  check_mem(marker);

  /*----------------------------------------------------------
  | Fill array of markers with line numbers, that  
  | contain the filter string
  ----------------------------------------------------------*/
  for (i = 0; i < txtlist->qty; i++) 
  {
    if ( binstr(fl_ptr[i], 0, bfltr) != BSTR_ERR )
    {
      hits += 1;
      marker[i] = i;
    }
    else
    {
      marker[i] = -1;
    }
  }

  /*----------------------------------------------------------
  | Create new bstrList that will contained all filtered lines
  ----------------------------------------------------------*/
  bl = bstrListCreate();
  bstrListAlloc(bl, hits);

  /*----------------------------------------------------------
  | Copy marked lines into new bstrList
  ----------------------------------------------------------*/
  bl_ptr = bl->entry;

  for (i = 0; i < txtlist->qty; i++) 
  {
    if (marker[i] >= 0)
    {
      const int curline = marker[i];
      bl_ptr[cnt] = bstrcpy(fl_ptr[curline]);
      bl->qty += 1;
      cnt += 1;
    }
  }

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy(bfltr);
  free(marker);

  return bl;

error:
  bdestroy(bfltr);
  free(marker);
  return NULL;
}


/*************************************************************
* Function searches for a specifier in a bstrList
* and returns and writes an integer value if one 
* is found behind the specifier
* Returns 0 if specifier was not found in the file.
* Otherwise, it returns the number of times, the 
* specifier was found.
* Returns -1 on errors.
*************************************************************/
int Txtio_intParam(struct bstrList *txtlist,
                   const char *fltr,
                   int *value)
{
  int nfound = 0;
  bstring line, valstr;
  struct bstrList *fltTxt = NULL;
  struct bstrList *blstextr = NULL;
  bstring bfltr, bextr;

  /*----------------------------------------------------------
  | Get all lines, containing the specifier
  ----------------------------------------------------------*/
  fltTxt = Txtio_getLinesWith(txtlist, fltr);
  nfound = fltTxt->qty;

  /*----------------------------------------------------------
  | Return if specifier is not found
  ----------------------------------------------------------*/
  if (fltTxt->qty < 1)
  {
    bstrListDestroy(fltTxt);
    return 0;
  }

  /*----------------------------------------------------------
  | Take last string, in which specifier was found
  ----------------------------------------------------------*/
  line  = fltTxt->entry[fltTxt->qty - 1];
  bfltr = bfromcstr( fltr ); 
  
  int off = binstr(line, 0, bfltr); 
  int len = bfltr->slen;

  bextr = bmidstr( line, off+len, line->slen );

  /*----------------------------------------------------------
  | Remove lasting whitespaces and copy first value
  ----------------------------------------------------------*/
  blstextr = bsplit(bextr, '\n');
  valstr = blstextr->entry[0];
  *value = atoi(valstr->data);

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( blstextr );

  return nfound;

error:
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( blstextr );
  return -1;
}

/*************************************************************
* Function searches for a specifier <fltr> in a bstrList.
* The parameter behind the specifier is then extracted 
* from the file and stored into <value>.
* The value is casted to a prescribed type
* type = 0: integer
* type = 1: double
* type = 2: string
*
* Returns 0 if specifier was not found in the file.
* Otherwise, it returns the number of times, the 
* specifier was found.
* Returns -1 on errors.
*************************************************************/
int Txtio_extractParam(struct bstrList *txtlist,
                       const char *fltr, int type,
                       void *value)
{
  int i;
  int nfound = 0;
  bstring line, valstr;
  struct bstrList *fltTxt = NULL;
  struct bstrList *blstextr = NULL;
  bstring bfltr, bextr;

  /*----------------------------------------------------------
  | Get all lines, containing the specifier
  ----------------------------------------------------------*/
  fltTxt = Txtio_getLinesWith(txtlist, fltr);
  nfound = fltTxt->qty;

  /*----------------------------------------------------------
  | Return if specifier is not found
  ----------------------------------------------------------*/
  if (fltTxt->qty < 1)
  {
    bstrListDestroy(fltTxt);
    return 0;
  }

  /*----------------------------------------------------------
  | Take last string, in which specifier was found
  ----------------------------------------------------------*/
  line  = fltTxt->entry[fltTxt->qty - 1];
  bfltr = bfromcstr( fltr ); 
  
  int off = binstr(line, 0, bfltr); 
  int len = bfltr->slen;

  bextr = bmidstr( line, off+len, line->slen );

  /*----------------------------------------------------------
  | Remove leading whitespaces and copy first value
  ----------------------------------------------------------*/
  valstr = bextr;

  if (type == 0)
    *(int*)value = atoi(valstr->data);
  else if (type == 1)
    *(double*)value = atof(valstr->data);
  else if (type == 2)
    *(bstring*)value = bfromcstr( valstr->data );
  else
    log_err("Wrong type definition.");

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( blstextr );

  return nfound;

error:
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( blstextr );
  return -1;
}
