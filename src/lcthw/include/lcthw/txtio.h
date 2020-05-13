#ifndef _lcthw_Txtio_h
#define _lcthw_Txtio_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lcthw/bstrlib.h>

#define FILIO_ERR -1

/*************************************************************
* Txtio structure
*************************************************************/
struct Txtio;
typedef struct Txtio {
  const char      *path;    /* Path of file                 */
  bstring          txt;     /* bstring with file data       */
  struct bstrList *txtlist; /* file, splitted for newlines  */

  long             length;  /* Number of chars in total file*/
                            /* -> including '\0' at end     */
  int              nlines;  /* Number of lines in total file*/


} Txtio;

/*************************************************************
* Function to create a new file reader structure
*************************************************************/
Txtio *Txtio_create(const char *file_path);

/*************************************************************
* Function to destroy a file reader structure
*************************************************************/
int Txtio_destroy(Txtio *file);

/*************************************************************
* Function returns a bstring list of lines, that 
* do not contain a certain specifier
*************************************************************/
struct bstrList *Txtio_popLinesWith(struct bstrList *txtlist,
                                    const char *fltr);

/*************************************************************
* Function returns a bstring list of lines, that 
* do contain a certain specifier
*************************************************************/
struct bstrList *Txtio_getLinesWith(struct bstrList *txtlist,
                                    const char *fltr);

/*************************************************************
* Function searches for a specifier in a bstrList
* and returns and writes an integer value if one 
* is found behind the specifier
* Returns 0 if integer was found and -1 if not
*************************************************************/
int Txtio_intParam(struct bstrList *txtlist,
                   const char *fltr,
                   int *value);

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
                       void *value);


#endif
