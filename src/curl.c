/** @file curl.c
 *  @author T J Atherton
 *
 *  @brief Wrapper for cURL library
*/

#include <morpho.h>
#include <morpho/classes.h>

#include <curl/curl.h>

#include "curl.h"

/* **********************************************************************
 * Curl objects
 * ********************************************************************** */

/** Curl object definitions */
void objectcurl_printfn(object *obj, void *v) {
    morpho_printf(v, "<Curl>");
}

void objectcurl_markfn(object *obj, void *v) {
}

size_t objectcurl_sizefn(object *obj) {
    return sizeof(objecttuple);
}

objecttypedefn objectcurldefn = {
    .printfn = objectcurl_printfn,
    .markfn = objectcurl_markfn,
    .freefn = NULL,
    .sizefn = objectcurl_sizefn,
    .hashfn = NULL,
    .cmpfn = NULL
};

/** @brief Creates a new curl object with a collection of URLs
 *  @param n number of URLs
 *  @param urls list of URLs
 *  @returns the object or NULL on failure */
objectcurl *object_newcurl(int n, value *urls) {
    objectcurl *new = (objectcurl *) object_new(sizeof(objectcurl), OBJECT_CURL);

    if (new) {
        
    }
    
    return new;
}

/* **********************************************************************
 * Curl class
 * ********************************************************************** */

/* -------------------------------------------------------
 * Constructors
 * ------------------------------------------------------- */

/** Constructor function for Curl */
value curl_constructor(vm *v, int nargs, value *args) {
    value out = MORPHO_NIL;
    
    for (int i=0; i<nargs; i++) { // Check that arguments are all strings
        if (!MORPHO_ISSTRING(MORPHO_GETARG(args, 0))) {
            morpho_runtimeerror(v, CURL_ARGS);
            return MORPHO_NIL;
        }
    }
    
    objectcurl *new = object_newcurl(nargs, & MORPHO_GETARG(args, 0));
    
    if (new) {
        out=MORPHO_OBJECT(new);
        morpho_bindobjects(v, 1, &out);
    }
    
    return out;
}

/* -------------------------------------------------------
 * Methods
 * ------------------------------------------------------- */

value Curl_fetch(vm *v, int nargs, value *args) {
    return MORPHO_NIL;
}

MORPHO_BEGINCLASS(Curl)
MORPHO_METHOD(CURL_FETCH_METHOD, Curl_fetch, BUILTIN_FLAGSEMPTY)
MORPHO_ENDCLASS

/* **********************************************************************
 * Initialization and finalization
 * ********************************************************************** */

objecttype objectcurltype;

void curl_initialize(void) {
    curl_global_init(CURL_GLOBAL_ALL);

    // Create curl object type
    objectcurltype=object_addtype(&objectcurldefn);
    
    objectstring objclassname = MORPHO_STATICSTRING(OBJECT_CLASSNAME);
    value objclass = builtin_findclass(MORPHO_OBJECT(&objclassname));
    
    // Create the Curl class
    value curlclass = builtin_addclass(CURL_CLASSNAME, MORPHO_GETCLASSDEFINITION(Curl), MORPHO_NIL);
    object_setveneerclass(OBJECT_CURL, curlclass);
    
    // Curl constructor function
    morpho_addfunction(CURL_CLASSNAME, "Curl (...)", curl_constructor, MORPHO_FN_CONSTRUCTOR, NULL);
    
    morpho_defineerror(CURL_ARGS, ERROR_HALT, CURL_ARGS_MSG);
}

void curl_finalize(void) { 
    curl_global_cleanup();
}
