#include "tmesh/dbg.h"
#include "tmesh/bstrlib.h"
#include "tmesh/tmParam.h"

/*************************************************************
* Function to create a new parameter file reader
*
* Reference:
* https://stackoverflow.com/questions/14002954/c-programming
* -how-to-read-the-whole-file-contents-into-a-buffer
*************************************************************/
tmParam *tmParam_create(const char *file_path)
{
  /*---------------------------------------------------------
  | Allocate memory for txtio structure 
  ---------------------------------------------------------*/
  tmParam *txtfile = calloc(1, sizeof(tmParam));
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

  /* Copy relevant data to tmParam structure               */
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
int tmParam_destroy(tmParam *file)
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
struct bstrList *tmParam_popLinesWith(struct bstrList *txtlist, 
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
struct bstrList *tmParam_getLinesWith(struct bstrList *txtlist,
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
int tmParam_extractParam(struct bstrList *txtlist,
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
  fltTxt = tmParam_getLinesWith(txtlist, fltr);
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

/*************************************************************
* Function searches for a specifier <fltr> in a bstrList.
* The string behind the specifier is then extracted 
* from the file and processed as an array of values 
* and stored in <value>.
* The values are casted to a prescribed type
* type = 0: integer
* type = 1: double
* type = 2: bstrList (not working )
*
* Returns 0 if specifier was not found in the file.
* Otherwise, it returns the number of times, the 
* specifier was found.
* Returns -1 on errors.
*************************************************************/
int tmParam_extractArray(struct bstrList *txtlist,
                         const char *fltr, int type,
                         void *value)
{

  int i;
  int nfound = 0;
  bstring line;
  struct bstrList *fltTxt = NULL;
  bstring bfltr, bextr;

  /*----------------------------------------------------------
  | Get all lines, containing the specifier
  ----------------------------------------------------------*/
  fltTxt = tmParam_getLinesWith(txtlist, fltr);
  nfound = fltTxt->qty;

  /*----------------------------------------------------------
  | Return if specifier is not found
  ----------------------------------------------------------*/
  if (nfound < 1)
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
  bstring wsfnd = bfromcstr( " " );
  bstring wsrpl = bfromcstr( "" );
  bfindreplace(bextr, wsfnd, wsrpl, 0);

  /*----------------------------------------------------------
  | Split into list of string -> comma is separator
  ----------------------------------------------------------*/
  struct bstrList *arrStr = bsplit(bextr, ',');
  int nEntries            = arrStr->qty;
  bstring *arr_ptr        = arrStr->entry;

  if (type == 0)
  {
    int *array = calloc(nEntries, sizeof(int));

    for (i = 0; i < nEntries; i++) 
      array[i] = atoi(arr_ptr[i]->data);

    *(int**)value = array;

  }
  else if (type == 1)
  {
    double *array = calloc(nEntries, sizeof(double));

    for (i = 0; i < nEntries; i++) 
      array[i] = atof(arr_ptr[i]->data);

    *(double**)value = array;

  }
  else if (type == 2)
  {
    *(struct bstrList**)value = arrStr;
  }
  else
    log_err("Wrong type definition.");

  /*----------------------------------------------------------
  | Cleanup
  ----------------------------------------------------------*/
  bdestroy( wsfnd );
  bdestroy( wsrpl );
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  bstrListDestroy( arrStr);

  return nfound;

error:
  bdestroy( bextr );
  bdestroy( bfltr );
  bstrListDestroy( fltTxt );
  return -1;

} /* tmParam_extractArray() */

/*************************************************************
* Function to extract the node coordinates from a parameter 
* file
*************************************************************/
int tmParam_readNodeCoords(struct bstrList *txtlist,
                           tmDouble       (**xy)[2], 
                           int             *n)
{
  tmDouble (*coords)[2];

  int nNodes, nValues, i;
  int i_start = 0;
  int i_end   = 0;
  int cnt     = 0;

  struct bstrList *values;
  bstring *val_ptr;
  bstring wsfnd, wsrpl, line;

  bstring *fl_ptr = txtlist->entry;
  bstring  bstart  = bfromcstr( "Define nodes:" );
  bstring  bend    = bfromcstr( "End nodes" );

  /*----------------------------------------------------------
  | Fill array of markers with line numbers, that  
  | contain the start and ending string
  ----------------------------------------------------------*/
  for (i = 0; i < txtlist->qty; i++) 
  {
    if ( binstr(fl_ptr[i], 0, bstart) != BSTR_ERR )
      i_start = i;

    if ( binstr(fl_ptr[i], 0, bend) != BSTR_ERR )
      i_end = i;
  }

  /*----------------------------------------------------------
  | Estimate number of nodes
  ----------------------------------------------------------*/
  check( i_start < i_end, 
      "Wrong definition of node coordinates." );

  nNodes = i_end - i_start - 1;
  coords = calloc(nNodes, 2*sizeof(tmDouble));

  for (i = i_start+1; i < i_end; i++)
  {
    line  = fl_ptr[i];
    wsfnd = bfromcstr( " " );
    wsrpl = bfromcstr( "" );
    bfindreplace(line, wsfnd, wsrpl, 0);

    values  = bsplit(line, ',');
    nValues = values->qty;
    val_ptr = values->entry;

    check (nValues == 2, 
        "Wrong definition for node coordinates.");

    coords[cnt][0] = atof(val_ptr[0]->data);
    coords[cnt][1] = atof(val_ptr[1]->data);
    cnt++;

    bdestroy( wsfnd );
    bdestroy( wsrpl );
    bstrListDestroy( values );
  }

  *xy = coords;
  *n  = nNodes;

  bdestroy( bstart );
  bdestroy( bend );

  return 1;


error:

  bdestroy( bstart );
  bdestroy( bend );
  bdestroy( wsfnd );
  bdestroy( wsrpl );
  bstrListDestroy( values );
  free(coords);
  return -1;

} /* tmParam_readNodesCoords() */



/*************************************************************
* Function to extract the exterior boundary data 
* from a parameter file
*************************************************************/
int tmParam_readBdryData(struct bstrList *txtlist,
                         char       *startStr,
                         char       *endStr,
                         int         startLine,
                         tmBool      readAll,
                         int       (**edges)[2], 
                         int        **edgeMarker, 
                         tmDouble   **edgeRefine,
                         int         *nEdges,
                         int         *bdryMarker)
{
  int     (*edg)[2];
  int      *mark;
  tmDouble *ref;

  int nEdge, nValues, i, bdryMark;
  int i_start = 0;
  int i_end   = 0;
  int cnt     = 0;

  struct bstrList *values;
  bstring *val_ptr;
  bstring wsfnd, wsrpl, line, bextr, curLine;

  bstring *fl_ptr = txtlist->entry;
  bstring  bstart  = bfromcstr( startStr );
  bstring  bend    = bfromcstr( endStr );

  tmParam_extractParam(txtlist, startStr, 
                       0, &bdryMark);

  int off;

  /*----------------------------------------------------------
  | Fill array of markers with line numbers, that  
  | contain the start and ending string
  ----------------------------------------------------------*/
  for (i = startLine; i < txtlist->qty; i++) 
  {

    if ( (off = binstr(fl_ptr[i], 0, bstart)) != BSTR_ERR )
    {
      i_start = i;

      curLine = fl_ptr[i];
      int len = bstart->slen;

      bextr = bmidstr( curLine, off+len, curLine->slen );
      bdryMark = atoi(bextr->data);

    }

    if ( binstr(fl_ptr[i], 0, bend) != BSTR_ERR )
    {
      i_end = i;
      if (readAll == FALSE)
        break;
    }
  }

  /*----------------------------------------------------------
  | Estimate number of nodes
  ----------------------------------------------------------*/
  check( i_start < i_end, 
      "Wrong definition of node coordinates." );

  nEdge = i_end - i_start - 1;
  edg  = calloc(nEdge, 2*sizeof(int));
  mark = calloc(nEdge, sizeof(int));
  ref  = calloc(nEdge, sizeof(tmDouble));

  for (i = i_start+1; i < i_end; i++)
  {
    line  = fl_ptr[i];
    wsfnd = bfromcstr( " " );
    wsrpl = bfromcstr( "" );
    bfindreplace(line, wsfnd, wsrpl, 0);

    values  = bsplit(line, ',');
    nValues = values->qty;
    val_ptr = values->entry;

    check (nValues == 4, 
        "Wrong definition for exterior edges.");

    edg[cnt][0] = atoi(val_ptr[0]->data);
    edg[cnt][1] = atoi(val_ptr[1]->data);
    mark[cnt]   = atoi(val_ptr[2]->data);
    ref[cnt]    = atof(val_ptr[3]->data);
    cnt++;

    bdestroy( wsfnd );
    bdestroy( wsrpl );
    bstrListDestroy( values );
  }

  *edges       = edg;
  *edgeMarker  = mark;
  *edgeRefine  = ref;
  *nEdges      = nEdge;
  *bdryMarker  = bdryMark;

  bdestroy( bstart );
  bdestroy( bend );
  bdestroy( bextr );

  return 1;


error:

  bdestroy( bstart );
  bdestroy( bend );
  bdestroy( wsfnd );
  bdestroy( wsrpl );
  bstrListDestroy( values );
  free(edg);
  free(mark);
  free(ref);
  return -1;

} /* tmParam_readExtBdryData() */

/*************************************************************
* Function to extract the exterior boundary data 
* from a parameter file
*************************************************************/
int tmParam_readIntBdryData(struct bstrList *txtlist,
                            int       (***edges)[2],
                            int        ***edgeMarker, 
                            tmDouble   ***edgeRefine,
                            int         **nEdges,
                            int         **bdryMarkers,
                            int          *nBdrys)
{
  int i, nBdry, cur;
  int n_start = 0;
  int n_end   = 0;

  int     (**edg)[2];
  int      **mark;
  tmDouble **ref;
  int       *nEdgAll;
  int       *markAll;

  bstring *fl_ptr = txtlist->entry;
  bstring  bstart  = bfromcstr( "Define interior boundary:" );
  bstring  bend    = bfromcstr( "End interior boundary" );

  /*----------------------------------------------------------
  | Estimate number of interior boundaries
  ----------------------------------------------------------*/
  for (i = 0; i < txtlist->qty; i++) 
  {
    if ( binstr(fl_ptr[i], 0, bstart) != BSTR_ERR )
      n_start += 1;

    if ( binstr(fl_ptr[i], 0, bend) != BSTR_ERR )
      n_end += 1;
  }

  check( n_start == n_end, 
      "Wrong definition of interior boundaries.");

  tmPrint("%d INTERIOR BOUNDARIES FOUND.", n_start);

  /*----------------------------------------------------------
  | Allocate memory
  ----------------------------------------------------------*/
  nBdry   = n_start;
  edg     = calloc(nBdry, sizeof(int**));
  mark    = calloc(nBdry, sizeof(int*));
  ref     = calloc(nBdry, sizeof(tmDouble*));
  nEdgAll = calloc(nBdry, sizeof(int));
  markAll = calloc(nBdry, sizeof(int));

  /*----------------------------------------------------------
  | Gather data
  ----------------------------------------------------------*/
  cur = 0;
  for (i = 0; i < txtlist->qty; i++) 
  {
    if ( binstr(fl_ptr[i], 0, bstart) != BSTR_ERR )
    {
      tmParam_readBdryData(txtlist, 
                           "Define interior boundary:",
                           "End interior boundary",
                           i,
                           FALSE,
                           &edg[cur], &mark[cur],
                           &ref[cur], &nEdgAll[cur],
                           &markAll[cur]);
      cur++;
    }
  }

  /*----------------------------------------------------------
  | Handle data to argument pointers
  ----------------------------------------------------------*/
  *edges       = edg;
  *edgeMarker  = mark;
  *edgeRefine  = ref;
  *nEdges      = nEdgAll;
  *bdryMarkers = markAll;
  *nBdrys      = nBdry;


  bdestroy( bstart );
  bdestroy( bend );

  return 1;


error:
  bdestroy( bstart );
  bdestroy( bend );
  free(edg);
  free(mark);
  free(ref);
  free(nEdgAll);
  free(markAll);
  return -1;


} /* tmParam_readIntBdryData() */

