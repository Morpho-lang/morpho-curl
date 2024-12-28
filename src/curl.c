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
    objectcurl *curl = (objectcurl *) obj;
    morpho_markvarrayvalue(v, &curl->urls);
}

void objectcurl_freefn(object *obj) {
    objectcurl *curl = (objectcurl *) obj;
    varray_valueclear(&curl->urls);
}

size_t objectcurl_sizefn(object *obj) {
    objectcurl *curl = (objectcurl *) obj;
    return sizeof(objecttuple) + sizeof(value)*curl->urls.capacity;
}

objecttypedefn objectcurldefn = {
    .printfn = objectcurl_printfn,
    .markfn = objectcurl_markfn,
    .freefn = objectcurl_freefn,
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
        varray_valueinit(&new->urls);
        varray_valueadd(&new->urls, urls, n);
    }
    
    return new;
}

/* **********************************************************************
 * Curl interface
 * ********************************************************************** */

/** Raises an error given a curl error code */
void morphocurl_error(vm *v, CURLcode code) {
    switch (code) {
        case CURLE_OK: return;
        default:
            morpho_runtimeerror(v, CURL_ERROR, curl_easy_strerror(code));
    }
}

static size_t _writecallback(char *ptr, size_t size, size_t n, void *ref) {
    varray_char *buffer = (varray_char *) ref;
    varray_charadd(buffer, ptr, (int) n);
    return n;
}

bool morphocurl_fetch(objectcurl *curlobj, value *out, CURLcode *result) {
    CURL *curl;
    CURLcode res = CURLE_COULDNT_RESOLVE_HOST;

    varray_char buffer;
    varray_charinit(&buffer);
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, MORPHO_GETCSTRING(curlobj->urls.data[0]));
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writecallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buffer);
        
        res = curl_easy_perform(curl);

        if (res==CURLE_OK) *out = object_stringfromvarraychar(&buffer);
        
        curl_easy_cleanup(curl);
    }
    
    if (result) *result=res;
    varray_charclear(&buffer);
    
    return (res==CURLE_OK);
}

/* **********************************************************************
 * Curl class
 * ********************************************************************** */

/* -------------------------------------------------------
 * Constructors
 * ------------------------------------------------------- */

/** Constructor function for Curl */
value Curl_constructor(vm *v, int nargs, value *args) {
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
    objectcurl *slf = CURL_GETCURL(MORPHO_SELF(args));
    value out=MORPHO_NIL;
    
    CURLcode res;
    if (!morphocurl_fetch(slf, &out, &res)) morphocurl_error(v, res);
    
    if (MORPHO_ISOBJECT(out)) morpho_bindobjects(v, 1, &out);
    
    return out;
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
    morpho_addfunction(CURL_CLASSNAME, "Curl (...)", Curl_constructor, MORPHO_FN_CONSTRUCTOR, NULL);
    
    morpho_defineerror(CURL_ARGS, ERROR_HALT, CURL_ARGS_MSG);
    morpho_defineerror(CURL_ERROR, ERROR_HALT, CURL_ERROR_MSG);
}

void curl_finalize(void) { 
    curl_global_cleanup();
}
