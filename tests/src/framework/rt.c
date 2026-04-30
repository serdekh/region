#include "rt.h"

void *rt_try_load_shared_object(const char *file_path)
{
    void *handle = dlopen(file_path, RTLD_LAZY);
    
    if (!handle) {
        RT_LOG_ERROR_LINE("Could not load a dynamic executable: %s. Stop.", dlerror());
    }

    return handle;
}

void *rt_try_get_symbol(void *handle, const char *symbol_name, bool *is_error)
{
    if (!handle || !symbol_name) {
        RT_LOG_ERROR_LINE("Invalid arguments for symbol lookup.");
        if (is_error) *is_error = true;
        return NULL;
    }

#ifdef _WIN32
    FARPROC proc = GetProcAddress((HMODULE)handle, symbol_name);
    if (!proc) {
        RT_LOG_ERROR("GetProcAddress failed for '%s': %s",
            symbol_name, dlerror());
        if (is_error) *is_error = true;
        return NULL;
    }
    return (void*)proc;
#else
    dlerror();
    void *sym = dlsym(handle, symbol_name);
    const char *err = dlerror();

    if (err != NULL) {
        RT_LOG_ERROR_LINE("dlsym failed for '%s': %s", symbol_name, err);
        if (is_error) *is_error = true;
        return NULL;
    }

    return sym;
#endif
}

#define UNWRAP if (is_error) goto fatal

RegionAPI *rt_try_get_region_api_handle(void *handle)
{
    RegionAPI *region_api = (RegionAPI *)malloc(sizeof(RegionAPI));

    if (!region_api) {
        RT_LOG_ERROR_LINE("Could not allocate memory for the region api handle.");
        return NULL;
    }

    bool is_error = false;

    region_api->region_alloc           = rt_try_get_symbol(handle, SYMBOL_FN_REGION_ALLOC, &is_error); UNWRAP;
    region_api->region_clone           = rt_try_get_symbol(handle, SYMBOL_FN_REGION_CLONE, &is_error); UNWRAP;
    region_api->region_collect         = rt_try_get_symbol(handle, SYMBOL_FN_REGION_COLLECT, &is_error); UNWRAP;
    region_api->region_free            = rt_try_get_symbol(handle, SYMBOL_FN_REGION_FREE, &is_error); UNWRAP;
    region_api->region_get_last_node   = rt_try_get_symbol(handle, SYMBOL_FN_REGION_GET_LAST_NODE, &is_error); UNWRAP;
    region_api->region_merge           = rt_try_get_symbol(handle, SYMBOL_FN_REGION_MERGE, &is_error); UNWRAP;
    region_api->region_shrink_capacity = rt_try_get_symbol(handle, SYMBOL_FN_REGION_SHRINK_CAPACITY, &is_error); UNWRAP;

    region_api->region_push            = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH, &is_error); UNWRAP;
    region_api->region_push_int        = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_INT, &is_error); UNWRAP;
    region_api->region_push_float      = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_FLOAT, &is_error); UNWRAP;
    region_api->region_push_double     = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_DOUBLE, &is_error); UNWRAP;
    region_api->region_push_char       = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_CHAR, &is_error); UNWRAP;

    region_api->stack_region_alloc   = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_ALLOC, &is_error); UNWRAP;
    region_api->stack_region_free    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_FREE, &is_error); UNWRAP;
    region_api->stack_region_peek    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PEEK, &is_error); UNWRAP;
    region_api->stack_region_peek_at = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PEEK_AT, &is_error); UNWRAP;
    region_api->stack_region_swap    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_SWAP, &is_error); UNWRAP;
    
    region_api->stack_region_push        = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH, &is_error); UNWRAP;
    region_api->stack_region_push_int    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_INT, &is_error); UNWRAP;
    region_api->stack_region_push_float  = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_FLOAT, &is_error); UNWRAP;
    region_api->stack_region_push_double = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_DOUBLE, &is_error); UNWRAP;
    region_api->stack_region_push_char   = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_CHAR, &is_error); UNWRAP;

    region_api->stack_region_pop        = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP, &is_error); UNWRAP;
    region_api->stack_region_pop_int    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP_INT, &is_error); UNWRAP;
    region_api->stack_region_pop_float  = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP_FLOAT, &is_error); UNWRAP;
    region_api->stack_region_pop_double = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP_DOUBLE, &is_error); UNWRAP;

    region_api->test_set_available_memory = rt_try_get_symbol(handle, SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY, &is_error); UNWRAP; 
    return region_api;

fatal:
    if (region_api) free(region_api);
    RT_LOG_ERROR_LINE("Could not initialize the region api handle. Stop.");
    return NULL;
}

bool rt_try_load_file_and_test(RegionAPI *api, const char *file_path)
{
    if (!api || !file_path) {
        RT_LOG_ERROR_LINE("Could not load a file and test: invalid arguments.");
        return false;
    }

    bool is_error = false;

    void *handle = NULL;
    TestContext *(*get_start)(void) = NULL;
    TestContext *(*get_end)(void)   = NULL;

    handle = rt_try_load_shared_object(file_path); if (!handle) goto fatal;

    get_start = rt_try_get_symbol(handle, RT_GET_TEST_MODULE_START_STR, &is_error); if (is_error) goto fatal;
    get_end   = rt_try_get_symbol(handle, RT_GET_TEST_MODULE_END_STR, &is_error);   if (is_error) goto fatal;

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
    
    dlclose(handle); 
    return true;

fatal:
    if (handle) dlclose(handle);
    return false;
}

bool rt_try_load_files_and_test(const char *directory) 
{
    if (!directory) {
        RT_LOG_ERROR_LINE("Could not load files and test: no directory.");
        return false;
    }

    struct dirent *entry = NULL;

    DIR *dir = opendir(directory);

    if (!dir) {
        RT_LOG_ERROR_LINE("Could not open a directory \"%s\": %s.", directory, strerror(errno));
        return false;
    }

    void *region_api_handle = rt_try_load_shared_object(RT_FILE_PATHS_REGION_SO);

    if (!region_api_handle) {
        closedir(dir);
        return false;
    }

    RegionAPI *api = rt_try_get_region_api_handle(region_api_handle);

    if (!api) {
        closedir(dir);
        dlclose(region_api_handle);
        return false;
    }

    while (entry = readdir(dir)) {
    	char full_path[1024];
    	snprintf(full_path, sizeof(full_path), "%s%s%s", directory, RT_FILE_PATHS_SEP, entry->d_name);
    	
        struct stat st;
        
		if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
    		continue;
		}

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        bool test_result = rt_try_load_file_and_test(api, full_path);

        if (!test_result) {
            closedir(dir);
            free(api);
            dlclose(region_api_handle);
            return false;
        }
    }
    
    closedir(dir);
    dlclose(region_api_handle);
    free(api);
    return true;
}
