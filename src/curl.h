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

/** Tests whether an object is a tuple */
#define CURL_ISCURL(val) object_istype(val, OBJECT_CURL)

/** Extracts the objecttuple from a value */
#define CURL_GETCURL(val) ((objectcurl *) MORPHO_GETOBJECT(val))

/* -------------------------------------------------------
 * Curl class
 * ------------------------------------------------------- */

#define CURL_CLASSNAME                   "Curl"

#define CURL_FETCH_METHOD                "fetch"
#define CURL_URLENCODE_METHOD            "urlEncode"
#define CURL_URLDECODE_METHOD            "urlDecode"

/* -------------------------------------------------------
 * Curl error messages
 * ------------------------------------------------------- */

#define CURL_ARGS                         "CurlArgs"
#define CURL_ARGS_MSG                     "Curl initializer requires a collection of URLs provided either as strings or as a List."

#define CURL_ERROR                        "CurlErr"
#define CURL_ERROR_MSG                    "Curl error '%s'."

/* -------------------------------------------------------
 * Curl interface
 * ------------------------------------------------------- */

#endif
