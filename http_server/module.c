#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "server.h"

char* module_dir;

struct server_module* module_open(const char* module_name){
    char* module_path;
    void *handle;
    void (*module_generate)(int);
    struct server_module* module;
    
/* Construct the full path of the module shared library we’ll try to
load. */
    module_path =
    (char*)xmalloc(strlen(module_dir) + strlen(module_name) + 2);
    sprintf (module_path, "%s/%s", module_dir, module_name);
    
    /* Attempt to open MODULE_PATH as a shared library. */
    handle = dlopen(module_path, RTLD_NOW);
    free(module_path);
    if(handle == NULL){
        if (handle == NULL) {
        /* Failed; either this path doesn’t exist or it isn’t a shared
        library. */
            return NULL;
    }
    /* Resolve the module_generate symbol from the shared library. */
    module_generate = (void (*) (int)) dlsym (handle, "module_generate");
    /* Make sure the symbol was found. */
        
    if
}
