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
    morpho_markdictionary(v, &curl->urls);
}

void objectcurl_freefn(object *obj) {
    objectcurl *curl = (objectcurl *) obj;
    dictionary_clear(&curl->urls);
}

size_t objectcurl_sizefn(object *obj) {
    objectcurl *curl = (objectcurl *) obj;
    return sizeof(objectcurl) + sizeof(dictionaryentry)*curl->urls.capacity;
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

/** Write given data to a character buffer */
static size_t _writecallback(char *ptr, size_t size, size_t n, void *ref) {
    if (!ref) return n;
    varray_char *buffer = (varray_char *) ref;
    varray_charadd(buffer, ptr, (int) n);
    return n;
}

bool morphocurl_fetch(objectcurl *curlobj, value *out, CURLcode *result) {
    if (curlobj->urls.count<1) return false;
    
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

static void _add_transfer(CURLM *cm, value url, varray_char *buffer) {
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, MORPHO_GETCSTRING(url));
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) buffer);
    //curl_easy_setopt(curl, CURLOPT_PRIVATE, MORPHO_GETCSTRING(url));
    curl_multi_add_handle(cm, curl);
}

bool morphocurl_multi_fetch(vm *v, objectcurl *curlobj, value *out, CURLcode *result) {
    int ntransfers = curlobj->urls.count;
    int nremaining = ntransfers;
    int maxParallel = (nremaining < 10 ? nremaining : 10);

    CURLM *curl;
    CURLMsg *msg;
    
    varray_char *buffers = MORPHO_MALLOC(ntransfers*sizeof(varray_char));
    if (!buffers) return false;
    for (int i=0; i<ntransfers; i++) varray_charinit(&buffers[i]);
    
    curl = curl_multi_init();
    
    curl_multi_setopt(curl, CURLMOPT_MAXCONNECTS, (long) maxParallel);
    
    int itransfer;
    for (itransfer=0; itransfer<maxParallel; itransfer++) {
        _add_transfer(curl, curlobj->urls.data[itransfer], &buffers[itransfer]);
    }
    
    do {
        int nrunning = 1;
        curl_multi_perform(curl, &nrunning);
        
        int msgq = 0;
        while ((msg = curl_multi_info_read(curl, &msgq))) {
            if(msg->msg == CURLMSG_DONE) {
                CURL *e = msg->easy_handle;
                *result = msg->data.result; /** @warning: broken  */
                //curl_easy_getinfo(e, CURLINFO_PRIVATE, &url);
                curl_multi_remove_handle(curl, e);
                curl_easy_cleanup(e);
                nremaining--;
            } else {
                // Only CURLMSG_DONE is defined so never get here.
            }
            
            if (itransfer < ntransfers) {
                _add_transfer(curl, curlobj->urls.data[itransfer], &buffers[itransfer]);
                itransfer++;
            }
        }
        
        if (nremaining) curl_multi_wait(curl, NULL, 0, 1000, NULL);
    } while (nremaining);
    
    objectdictionary *new = object_newdictionary();
    
    if (new) {
        for (int i=0; i<ntransfers; i++) {
            value str = object_stringfromvarraychar(&buffers[i]);
            dictionary_insert(&new->dict, curlobj->urls.data[i], str);
        }
        *out=MORPHO_OBJECT(new);
    }
    
    for (int i=0; i<ntransfers; i++) varray_charclear(&buffers[i]);
    MORPHO_FREE(buffers);
    
    curl_multi_cleanup(curl);
}

/* **********************************************************************
 * Curl class
 * ********************************************************************** */

/* -------------------------------------------------------
 * Constructors
 * ------------------------------------------------------- */

/** Checks that all members of a list of values are strings */
static bool _checkurls(int n, value *urls) {
    for (int i=0; i<n; i++) {
        if (!MORPHO_ISSTRING(urls[i])) return false;
    }
    return true;
}

/** Generic constructor */
static value curl_constructor(vm *v, int n, value *urls) {
    value out = MORPHO_NIL;
    
    if (!_checkurls(n, urls)){
        morpho_runtimeerror(v, CURL_ARGS);
        return MORPHO_NIL;
    }
    
    objectcurl *new = object_newcurl(n, urls);
    
    if (new) {
        out=MORPHO_OBJECT(new);
        morpho_bindobjects(v, 1, &out);
    }
    
    return out;
}

/** Constructor function for Curl */
value Curl_constructor(vm *v, int nargs, value *args) {
    return curl_constructor(v, nargs, &MORPHO_GETARG(args, 0));
}

/** Constructor function for Curl accepting a list of URLs */
value Curl_listconstructor(vm *v, int nargs, value *args) {
    objectlist *list = MORPHO_GETLIST(MORPHO_GETARG(args, 0));
    return curl_constructor(v, list->val.count, list->val.data);
}

/** Constructor function for Curl accepting a tuple of URLs */
value Curl_tupleconstructor(vm *v, int nargs, value *args) {
    objecttuple *tuple = MORPHO_GETTUPLE(MORPHO_GETARG(args, 0));
    return curl_constructor(v, tuple->length, tuple->tuple);
}

/* -------------------------------------------------------
 * Methods
 * ------------------------------------------------------- */

value Curl_fetch(vm *v, int nargs, value *args) {
    objectcurl *slf = CURL_GETCURL(MORPHO_SELF(args));
    value out=MORPHO_NIL;
    
    CURLcode res;
    if (slf->urls.count>1) {
        if (!morphocurl_multi_fetch(v, slf, &out, &res)) morphocurl_error(v, res);
    } else {
        if (!morphocurl_fetch(slf, &out, &res)) morphocurl_error(v, res);
    }
    
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
    
    // Curl constructor functions
    morpho_addfunction(CURL_CLASSNAME, "Curl (Tuple)", Curl_tupleconstructor, MORPHO_FN_CONSTRUCTOR, NULL);
    morpho_addfunction(CURL_CLASSNAME, "Curl (List)", Curl_listconstructor, MORPHO_FN_CONSTRUCTOR, NULL);
    morpho_addfunction(CURL_CLASSNAME, "Curl (...)", Curl_constructor, MORPHO_FN_CONSTRUCTOR, NULL);
    
    morpho_defineerror(CURL_ARGS, ERROR_HALT, CURL_ARGS_MSG);
    morpho_defineerror(CURL_ERROR, ERROR_HALT, CURL_ERROR_MSG);
}

void curl_finalize(void) { 
    curl_global_cleanup();
}
