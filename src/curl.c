/** @file curl.c
 *  @author T J Atherton
 *
 *  @brief Wrapper for cURL library
*/

#include <morpho.h>
#include <morpho/classes.h>

#include <curl/curl.h>

#include "curl.h"

/* -------------------------------------------------------
 * Curl constructor
 * ------------------------------------------------------- */

/** Constructor function for Curl */
value curl_constructor(vm *v, int nargs, value *args) {
    value out = MORPHO_NIL;
    return out;
}

/* -------------------------------------------------------
 * Curl class
 * ------------------------------------------------------- */

value Curl_fetch(vm *v, int nargs, value *args) {
    return MORPHO_NIL;
}

MORPHO_BEGINCLASS(Curl)
MORPHO_METHOD(CURL_FETCH_METHOD, Curl_fetch, BUILTIN_FLAGSEMPTY)
MORPHO_ENDCLASS

/* -------------------------------------------------------
 * Initialization and finalization
 * ------------------------------------------------------- */

void curl_initialize(void) { 
    curl_global_init(CURL_GLOBAL_ALL);
    
    objectstring objclassname = MORPHO_STATICSTRING(OBJECT_CLASSNAME);
    value objclass = builtin_findclass(MORPHO_OBJECT(&objclassname));
    
    // Create the Curl class
    builtin_addclass(CURL_CLASSNAME, MORPHO_GETCLASSDEFINITION(Curl), MORPHO_NIL);
    
    // Curl constructor function
    morpho_addfunction(CURL_CLASSNAME, "Curl (...)", curl_constructor, MORPHO_FN_CONSTRUCTOR, NULL);
    
    //morpho_defineerror(TUPLE_ARGS, ERROR_HALT, TUPLE_ARGS_MSG);
}

void curl_finalize(void) { 
    curl_global_cleanup();
}
