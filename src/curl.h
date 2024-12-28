/** @file curl.h
 *  @author T J Atherton
 *
 *  @brief Wrapper for cURL library
*/

#ifndef curl_h
#define curl_h

#include <morpho.h>

/* -------------------------------------------------------
 * Curl object
 * ------------------------------------------------------- */

extern objecttype objectcurltype;
#define OBJECT_CURL objectcurltype

/** A curl object */
typedef struct {
    object obj;
    
    varray_value urls;
} objectcurl;

/* -------------------------------------------------------
 * Curl class
 * ------------------------------------------------------- */

#define CURL_CLASSNAME                   "Curl"

#define CURL_FETCH_METHOD                "fetch"

/* -------------------------------------------------------
 * Curl error messages
 * ------------------------------------------------------- */

#define CURL_ARGS                         "CurlArgs"
#define CURL_ARGS_MSG                     "Curl must be initialized with a collection of URLs as strings."

/* -------------------------------------------------------
 * Curl interface
 * ------------------------------------------------------- */

#endif
