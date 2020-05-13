#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>


/***************************************************
 * MACRO: debug
 * -------------------------------------------------
 * Translates any use of 
 *   <debug("format", arg1, arg2> 
 * into an fprintf call to the stderr-function.
 *
 * stderr is a file stream to output all the 
 * errors.
 **************************************************/
#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG: %s:%d: " M "\n",\
    __FILE__, __LINE__, ##__VA_ARGS__)
#endif

/***************************************************
 * MACRO: clean_errno
 * -------------------------------------------------
 * Used in other macros to get a safe, readable 
 * version of errno.
 *
 * strerror-function: 
 * Returns a pointer to a string that describes the 
 * error passed in the argument
 **************************************************/
#define clean_errno() (errno == 0 ? "None" : strerror(errno))


/***************************************************
 * MACRO: log_err
 * -------------------------------------------------
 * Used to logging error messages to the user
 **************************************************/
#define log_err(M, ...) fprintf(stderr, \
    "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, \
    clean_errno(), ##__VA_ARGS__)


/***************************************************
 * MACRO: log_warn
 * -------------------------------------------------
 * Used to logging warning messages to the user
 **************************************************/
#define log_warn(M, ...) fprintf(stderr, \
    "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, \
    clean_errno(), ##__VA_ARGS__)


/***************************************************
 * MACRO: log_info
 * -------------------------------------------------
 * Used to logging info messages to the user
 **************************************************/
#define log_info(M, ...) fprintf(stderr, \
    "[INFO] (%s:%d) " M "\n", \
    __FILE__, __LINE__, ##__VA_ARGS__)


/***************************************************
 * MACRO: check
 * -------------------------------------------------
 * Will make sure, that condition A is true. 
 * If not, it will log the error M with variable
 * arguments for log_err and then jumps to the 
 * functions error: for cleanup
 **************************************************/
#define check(A, M, ...) if(!(A)) { \
  log_err(M, ##__VA_ARGS__); \
  errno=0; \
  goto error; }


/***************************************************
 * MACRO: sentinel
 * -------------------------------------------------
 * This macro should be placed in any part of a 
 * function that shouldn't run and, if it does,
 * it prints an error message and then jumps to 
 * the error: label.
 **************************************************/
#define sentinel(M, ...) { \
  log_err(M, ##__VA_ARGS__); \
  errno=0; \
  goto error; }


/***************************************************
 * MACRO: check_mem
 * -------------------------------------------------
 * Makes sure that a pointer is valid, and if it
 * isn't, it reports it as an error with
 * "Out of memory"
 **************************************************/
#define check_mem(A) check((A), "Out of memory.")


/***************************************************
 * MACRO: check_debug
 * -------------------------------------------------
 * Checks and handles an error, but if the error 
 * is common, then it doesn't bother reporting it
 **************************************************/
#define check_debug(A, M, ...) if(!(A)) { \
  debug(M, ##__VA_ARGS__); \
  errno=0; \
  goto error; }

#endif /* __dbg_h__ */
