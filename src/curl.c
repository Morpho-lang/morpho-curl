/** @file curl.c
 *  @author T J Atherton
 *
 *  @brief Wrapper for cURL library
*/

#include "curl.h"
#include <curl/curl.h>

CURL *curl;

/* -------------------------------------------------------
 * Initialization and finalization
 * ------------------------------------------------------- */

void curl_initialize(void) { 
    curl_global_init(CURL_GLOBAL_ALL);
}

void curl_finalize(void) { 
    curl_global_cleanup();
}
