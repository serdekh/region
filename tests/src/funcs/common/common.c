#include "common.h"

void *_RegionHandle = NULL; 

void try_get_region_handle()
{
    if (_RegionHandle) return;

    _RegionHandle = dlopen(REGION_SHARED_OBJECT_FILE_PATH, RTLD_LAZY);
    
    if (_RegionHandle) return;

    fprintf(stderr, "[Test][Error]: Could not load the region shared library (%s): %s\n",
        REGION_SHARED_OBJECT_FILE_PATH, dlerror());

    exit(1);
}

void *try_get_symbol(const char *symbol_name)
{
    try_get_region_handle();

    void *symbol = dlsym(_RegionHandle, symbol_name);
    
    if (!symbol) {
        fprintf(stderr, "[Test][Error]: Could not find the '%s' symbol (%s): %s\n",
            symbol_name, REGION_SHARED_OBJECT_FILE_PATH, dlerror());
        dlclose(_RegionHandle);
        exit(1);
    }
    
    return symbol;
}