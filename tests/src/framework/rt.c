#include "rt.h"

void *rt_so_open(const char *file_path)
{
    void *handle = NULL;

    #if defined(_WIN32)
        handle = (void *)LoadLibrary(file_path);
    #else
        handle = dlopen(file_path, RTLD_LAZY);
    #endif
    
    if (!handle) RT_LOG_ERROR_LINE("Could not load a shared object: '%s'", rt_so_get_error());

    return handle;
}

void *rt_so_get_symbol(void *handle, const char *symbol_name)
{
    if (!handle) {
        RT_LOG_ERROR_LINE("Could not get a symbol: a shared object is not loaded");
        return NULL;
    }

    if (!symbol_name) {
        RT_LOG_ERROR_LINE("Could not get a symbol: a symbol name is not provided");
        return NULL;
    }

    void *symbol = NULL;

    #if defined(_WIN32)      
        symbol = (void*)GetProcAddress((HMODULE)handle, symbol_name);
    #else
        symbol = dlsym(handle, symbol_name);
    #endif

    if (!symbol) {
        RT_LOG_ERROR_LINE("Could not find the symbol '%s': %s",
            symbol_name, rt_so_get_error());
    }

    return symbol;
}

char *rt_so_get_error()
{
    #if defined(_WIN32)
        static char buffer[256];

	    DWORD error = GetLastError();

    	FormatMessageA(
	        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	        NULL,
	        error,
	        0,
	        buffer,
	        sizeof(buffer),
	        NULL
	    );

    	return (char *)buffer;
    #else
        return dlerror();
    #endif
}

void  rt_so_close(void *handle)
{
    if (!handle) return;

    #if defined(_WIN32)
        FreeLibrary(handle);
    #else
        dlclose(handle);
    #endif
}

#define TRY_GET_REGION_API_FN_SYMBOL(api_fn_ptr, handle, api_fn_symbol) \
    (api_fn_ptr) = rt_so_get_symbol((handle), (api_fn_symbol)); if (!(api_fn_ptr)) goto fatal;

RegionAPI *rt_try_get_region_api_handle(void *handle)
{
    if (!handle) {
        RT_LOG_ERROR_LINE("Could not initialize the region api handle: a shared object is not provided");
        return NULL;
    }

    RegionAPI *region_api = (RegionAPI *)malloc(sizeof(RegionAPI));

    if (!region_api) {
        RT_LOG_ERROR_LINE("Could not initialize the region api handle: failed to allocate memory");
        return NULL;
    }

    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_alloc, handle, SYMBOL_FN_REGION_ALLOC);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_free, handle, SYMBOL_FN_REGION_FREE);

    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_push, handle, SYMBOL_FN_REGION_PUSH);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_push_int, handle, SYMBOL_FN_REGION_PUSH_INT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_push_float, handle, SYMBOL_FN_REGION_PUSH_FLOAT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_push_double, handle, SYMBOL_FN_REGION_PUSH_DOUBLE);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_push_char, handle, SYMBOL_FN_REGION_PUSH_CHAR);

    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_clone, handle, SYMBOL_FN_REGION_CLONE);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_collect, handle, SYMBOL_FN_REGION_COLLECT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_get_last_node, handle, SYMBOL_FN_REGION_GET_LAST_NODE);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_merge, handle, SYMBOL_FN_REGION_MERGE);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->region_shrink_capacity, handle, SYMBOL_FN_REGION_SHRINK_CAPACITY);

    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_alloc, handle, SYMBOL_FN_STACK_REGION_ALLOC);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_free, handle, SYMBOL_FN_STACK_REGION_FREE);

    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_peek, handle, SYMBOL_FN_STACK_REGION_PEEK);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_peek_at, handle, SYMBOL_FN_STACK_REGION_PEEK_AT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_swap, handle, SYMBOL_FN_STACK_REGION_SWAP);

    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_push, handle, SYMBOL_FN_STACK_REGION_PUSH);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_push_int, handle, SYMBOL_FN_STACK_REGION_PUSH_INT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_push_float, handle, SYMBOL_FN_STACK_REGION_PUSH_FLOAT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_push_double, handle, SYMBOL_FN_STACK_REGION_PUSH_DOUBLE);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_push_char, handle, SYMBOL_FN_STACK_REGION_PUSH_CHAR);

    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_pop, handle, SYMBOL_FN_STACK_REGION_POP);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_pop_int, handle, SYMBOL_FN_STACK_REGION_POP_INT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_pop_float, handle, SYMBOL_FN_STACK_REGION_POP_FLOAT);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_pop_double, handle, SYMBOL_FN_STACK_REGION_POP_DOUBLE);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->stack_region_pop_char, handle, SYMBOL_FN_STACK_REGION_POP_CHAR);

    TRY_GET_REGION_API_FN_SYMBOL(region_api->test_set_available_memory, handle, SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY);
    TRY_GET_REGION_API_FN_SYMBOL(region_api->test_set_default_memory, handle, SYMBOL_FN_TEST_SET_DEFAULT_MEMORY);

    return region_api;

fatal:
    RT_LOG_ERROR_LINE("Could not initialize the region api handle: failed find a function api symbol");

    if (region_api) free(region_api);

    return NULL;
}

bool rt_try_load_file_and_test(RegionAPI *api, const char *file_path)
{
    if (!api || !file_path) return false;

    if (!api) {
        RT_LOG_ERROR_LINE("Could not load and/or test a file: the region api handle is not provided");
        return false;
    }
    
    if (!file_path) {
        RT_LOG_ERROR_LINE("Could not load and/or test a file: the file path is not provided");
        return false;
    }

    void *handle = NULL;
    TestContext *(*get_start)(void) = NULL;
    TestContext *(*get_end)(void)   = NULL;

    handle    = rt_so_open(file_path); if (!handle) goto fatal;
    get_start = rt_so_get_symbol(handle, RT_GET_TEST_MODULE_START_STR); if (!get_start) goto fatal;
    get_end   = rt_so_get_symbol(handle, RT_GET_TEST_MODULE_END_STR);   if (!get_end)   goto fatal;

    for (TestContext *t = get_start(); t < get_end(); t++) {
        TestResult result = t->func(api);
    
        if (result.success) {
            RT_LOG_PASSED_TEST(t);
            continue;
        } 

        if (result.fatal) goto fatal;
        
        RT_LOG_FAILED_TEST(t, result);

        goto fatal;
    }
    
    rt_so_close(handle);
    return true;

fatal:
    RT_LOG_ERROR_LINE("Could not load and/or test a file: fatal error. Stopping the tests..");
  
    if (handle) rt_so_close(handle);

    return false;
}

bool rt_try_load_files_and_test(const char *directory) 
{
    if (!directory) {
        RT_LOG_ERROR_LINE("Could not test files in a directory: the directory path is not provided");
        return false;
    }

    bool result = true;

    struct dirent *entry = NULL;
    DIR *dir = NULL;
    void *region_api_handle = NULL;
    RegionAPI *api = NULL;

    dir = opendir(directory); if (!dir) { 
        RT_LOG_ERROR_LINE("Could not test files in a directory: failed to open a directory: %s", strerror(errno));
        result = false; goto cleanup; 
    }

    region_api_handle = rt_so_open(RT_FILE_PATHS_REGION_SO); if (!region_api_handle) { 
        RT_LOG_ERROR_LINE("Could not test files in a directory: failed to load the region api handle at '%s'", RT_FILE_PATHS_REGION_SO);
        result = false; goto cleanup; 
    }

    api = rt_try_get_region_api_handle(region_api_handle); if (!api) { 
        RT_LOG_ERROR_LINE("Could not test files in a directory: failed to init the region api handle");
        result = false; goto cleanup; 
    }

    while (entry = readdir(dir)) {
    	char full_path[1024];

    	snprintf(full_path, sizeof(full_path), "%s%s%s", directory, RT_FILE_PATHS_SEP, entry->d_name);
    	
        struct stat st;
        
		if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) continue;
		
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        bool test_result = rt_try_load_file_and_test(api, full_path); if (!test_result) { 
            RT_LOG_ERROR_LINE("Could not test files in a directory: failed to perfom a test for '%s'", full_path);
            result = false; goto cleanup; 
        }
    }
    
cleanup:
    if (dir) closedir(dir);
    if (region_api_handle) rt_so_close(region_api_handle);
    if (api) free(api);
    return result;
}
