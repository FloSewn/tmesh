/*
 * This header file is part of the tmesh library.  
 * This code was written by Florian Setzwein in 2020, 
 * and is covered under the MIT License
 * Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef TMESH_TMPARAM_H
#define TMESH_TMPARAM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tmesh/tmTypedefs.h"
#include "tmesh/bstrlib.h"

#define FILIO_ERR -1

/*************************************************************
* Parameter file structure
*************************************************************/
struct tmParam;
typedef struct tmParam {
  const char      *path;    /* Path of file                 */
  bstring          txt;     /* bstring with file data       */
  struct bstrList *txtlist; /* file, splitted for newlines  */

  long             length;  /* Number of chars in total file*/
                            /* -> including '\0' at end     */
  int              nlines;  /* Number of lines in total file*/

} tmParam;

/*************************************************************
* Function to create a new parameter file reader structure
*************************************************************/
tmParam *tmParam_create(const char *file_path);

/*************************************************************
* Function to destroy a file reader structure
*************************************************************/
int tmParam_destroy(tmParam *file);

/*************************************************************
* Function returns a bstring list of lines, that 
* do not contain a certain specifier
*************************************************************/
struct bstrList *tmParam_popLinesWith(struct bstrList *txtlist,
                                      const char *fltr);

/*************************************************************
* Function returns a bstring list of lines, that 
* do contain a certain specifier
*************************************************************/
struct bstrList *tmParam_getLinesWith(struct bstrList *txtlist,
                                      const char *fltr);

/*************************************************************
* This function removes all strings in the file that are 
* located behind a "comment" identifiert and the next 
* new line character
*************************************************************/
struct bstrList *tmParam_removeComments(struct bstrList *txtlist, 
                                        const char *fltr);

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
                         void *value);

/*************************************************************
* Function searches for a specifier <fltr> in a bstrList.
* The string behind the specifier is then extracted 
* from the file and processed as an array of values 
* and stored in <value>.
* The values are casted to a prescribed type
* type = 0: integer
* type = 1: double
* type = 2: string
*
* Returns 0 if specifier was not found in the file.
* Otherwise, it returns the number of times, the 
* specifier was found.
* Returns -1 on errors.
*************************************************************/
int tmParam_extractArray(struct bstrList *txtlist,
                         const char *fltr, int type,
                         void *value);

/*************************************************************
* Function to extract the node coordinates from a parameter 
* file
*************************************************************/
int tmParam_readNodeCoords(struct bstrList *txtlist,
                           tmDouble       (**xy)[2], 
                           int             *n);

/*************************************************************
* Function to extract the exterior boundary data 
* from a parameter file
*
* @param startStr : start identifier for which to search
* @param endStr   : end identifier when to stop searching
* @param startLine: index of line where to start
* @param readAll  : boolean if whole file will be read 
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
                         int         *bdryMarker);

/*************************************************************
* Function to extract the exterior boundary data 
* from a parameter file
*************************************************************/
int tmParam_readExtBdryData(struct bstrList *txtlist,
                            int       (**edges)[2],
                            int        **edgeMarker, 
                            tmDouble   **edgeRefine,
                            int         *nEdges,
                            int         *bdryMarkers);

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
                            int          *nBdrys);


#endif
