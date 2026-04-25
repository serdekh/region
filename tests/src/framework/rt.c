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
        RT_LOG_ERROR_LINE("Cannot look up a symbol name: invalid argument.");
        if (is_error) *is_error = true;
        return NULL;
    }

    void *symbol = dlsym(handle, symbol_name);
    
    if (!symbol) {
        RT_LOG_ERROR_LINE("Could not find a symbol `%s` in `%s`: %s.",
            symbol_name, RT_FILE_PATHS_REGION_SO, dlerror());
        if (is_error) *is_error = true;
        return NULL;
    }
    
    return symbol;
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
    region_api->region_push            = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH, &is_error); UNWRAP;
    region_api->region_push_int        = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_INT, &is_error); UNWRAP;
    region_api->region_push_float      = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_FLOAT, &is_error); UNWRAP;
    region_api->region_push_double     = rt_try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_DOUBLE, &is_error); UNWRAP;
    region_api->region_shrink_capacity = rt_try_get_symbol(handle, SYMBOL_FN_REGION_SHRINK_CAPACITY, &is_error); UNWRAP;

    region_api->stack_region_alloc   = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_ALLOC, &is_error); UNWRAP;
    region_api->stack_region_free    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_FREE, &is_error); UNWRAP;
    region_api->stack_region_peek    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PEEK, &is_error); UNWRAP;
    region_api->stack_region_peek_at = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PEEK_AT, &is_error); UNWRAP;
    region_api->stack_region_swap    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_SWAP, &is_error); UNWRAP;
    
    region_api->stack_region_push        = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH, &is_error); UNWRAP;
    region_api->stack_region_push_int    = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_INT, &is_error); UNWRAP;
    region_api->stack_region_push_float  = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_FLOAT, &is_error); UNWRAP;
    region_api->stack_region_push_double = rt_try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_DOUBLE, &is_error); UNWRAP;
    
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

char *append_file_to_directory(const char *directory, char *file_name)
{
    if (!directory || !file_name) {
        RT_LOG_ERROR_LINE("Failed to allocate memory while testing: no directory or file name.");
        return NULL;   
    }

    size_t file_name_len = strlen(file_name);
    size_t directory_len = strlen(directory);

    size_t file_path_len = file_name_len + directory_len + 2;

    char *file_path = (char *)malloc(sizeof(char) * file_path_len);

    if (!file_path) {
        RT_LOG_ERROR_LINE("Failed to allocate memory while testing.");
        return NULL;
    }

    memcpy(file_path, directory, directory_len);
    memcpy(file_path + directory_len, "/", 1);
    memcpy(file_path + directory_len + 1, file_name, file_name_len);
    memcpy(file_path + directory_len + 1 + file_name_len, "\0", 1);

    return file_path;
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
        if (entry->d_type == DT_DIR) continue;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char *file_path = append_file_to_directory(directory, entry->d_name);

        if (!file_path) {
            closedir(dir);
            free(api);
            dlclose(region_api_handle);
            return false;
        }

        bool test_result = rt_try_load_file_and_test(api, file_path);

        free(file_path);

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