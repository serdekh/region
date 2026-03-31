#include "common.h"

void *_RegionHandle = NULL; 
FuncPtr_test_set_available_memory __test_set_available_memory;
FuncPtr_test_get_available_memory __test_get_available_memory;

void try_get_region_handle()
{
    if (_RegionHandle) return;

    _RegionHandle = dlopen(REGION_SHARED_OBJECT_FILE_PATH, RTLD_LAZY);
    
    if (!_RegionHandle) {
        fprintf(stderr, "[Test][Error]: Could not load the region shared library (%s): %s\n",
            REGION_SHARED_OBJECT_FILE_PATH, dlerror());
        exit(1);
    }
}

void *try_get_symbol(const char *symbol_name)
{
    try_get_region_handle();

    void *symbol = dlsym(_RegionHandle, symbol_name);
    
    if (!symbol) {
        fprintf(stderr, "[Test][Error]: Could not find the '%s' symbol (%s): %s\n",
            symbol_name, REGION_SHARED_OBJECT_FILE_PATH, dlerror());
        if (_RegionHandle) dlclose(_RegionHandle);
        exit(1);
    }
    
    return symbol;
}

void set_available_memory(size_t value)
{
    if (!__test_set_available_memory) {
        __test_set_available_memory = try_get_symbol(SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY);
    }

    __test_set_available_memory(value);
}

size_t get_available_memory()
{
    if (!__test_get_available_memory) {
        __test_get_available_memory = try_get_symbol(SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY);
    }

    return __test_get_available_memory();
}