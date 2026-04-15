#include "common.h"

void *_RegionHandle = NULL; 
RegionAPI *region_api = NULL;

FuncPtr_test_set_available_memory __test_set_available_memory;
FuncPtr_test_get_available_memory __test_get_available_memory;

bool try_get_region_handle()
{
    if (_RegionHandle) return true;

    _RegionHandle = dlopen(REGION_SHARED_OBJECT_FILE_PATH, RTLD_LAZY);

    if (!_RegionHandle) {
        TEST_LOG(stderr, "Error", "Could not open the region shared library: %s", dlerror());
    }

    return _RegionHandle != NULL;
}

RegionAPI *try_get_region_api_handle()
{
    if (region_api) return region_api;

    if (!try_get_region_handle()) exit(1);

    region_api = (RegionAPI *)malloc(sizeof(RegionAPI));

    if (!region_api) {
        dlclose(_RegionHandle);
        TEST_LOG(stderr, "Error", "Could not allocate memory for the region api handle");
        exit(1);
    }

    region_api->region_alloc           = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
    region_api->region_clone           = try_get_symbol(SYMBOL_FN_REGION_CLONE);
    region_api->region_collect         = try_get_symbol(SYMBOL_FN_REGION_COLLECT);
    region_api->region_free            = try_get_symbol(SYMBOL_FN_REGION_FREE);
    region_api->region_get_last_node   = try_get_symbol(SYMBOL_FN_REGION_GET_LAST_NODE);
    region_api->region_merge           = try_get_symbol(SYMBOL_FN_REGION_MERGE);
    region_api->region_push            = try_get_symbol(SYMBOL_FN_REGION_PUSH);
    region_api->region_shrink_capacity = try_get_symbol(SYMBOL_FN_REGION_SHRINK_CAPACITY);

    region_api->stack_region_alloc   = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
    region_api->stack_region_free    = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    region_api->stack_region_peek    = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK);
    region_api->stack_region_peek_at = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK_AT);
    region_api->stack_region_pop     = try_get_symbol(SYMBOL_FN_STACK_REGION_POP);
    region_api->stack_region_push    = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    region_api->stack_region_swap    = try_get_symbol(SYMBOL_FN_STACK_REGION_SWAP);

    return region_api;
}

void *try_get_symbol(const char *symbol_name)
{
    if (!_RegionHandle) {
        if (!try_get_region_handle()) {
            exit(1);
        }
    }

    void *symbol = dlsym(_RegionHandle, symbol_name);
    
    if (!symbol) {
        TEST_LOG(stderr, "Error", "Could not find a symbol `%s` in `%s`: %s",
            symbol_name, REGION_SHARED_OBJECT_FILE_PATH, dlerror());
        dlclose(_RegionHandle);
        free(region_api);
        exit(1);
    }
    
    return symbol;
}

void close_region_api_handle()
{
    if (_RegionHandle) dlclose(_RegionHandle);
    if (region_api) free(region_api);
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