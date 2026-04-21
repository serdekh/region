#include "./tester.h"

void *try_load_shared_object(const char *file_path)
{
    void *handle = dlopen(file_path, RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "[Test][Error]: Could not load a dynamic executable (%s): %s\nStop.\n",
            file_path, dlerror());
    }

    return handle;
}

void *try_get_symbol(void *handle, const char *symbol_name, bool *is_error)
{
    if (!handle || !symbol_name) {
        TEST_LOG_LINE(stderr, "Error", "Cannot look up a symbol name: invalid argument.");
        if (is_error) *is_error = true;
        return NULL;
    }

    void *symbol = dlsym(handle, symbol_name);
    
    if (!symbol) {
        TEST_LOG_LINE(stderr, "Error", "Could not find a symbol `%s` in `%s`: %s",
            symbol_name, TEST_REGION_TEST_SO_FILE_PATH, dlerror());
        if (is_error) *is_error = true;
        return NULL;
    }
    
    return symbol;
}

#define REGION_API_HANDLE_UNWRAP if (is_error) goto fatal

RegionAPI *try_get_region_api_handle(void *handle)
{
    RegionAPI *region_api = (RegionAPI *)malloc(sizeof(RegionAPI));

    if (!region_api) {
        TEST_LOG_LINE(stderr, "Error", "Could not allocate memory for the region api handle");
        return NULL;
    }

    bool is_error = false;

    region_api->region_alloc           = try_get_symbol(handle, SYMBOL_FN_REGION_ALLOC, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_clone           = try_get_symbol(handle, SYMBOL_FN_REGION_CLONE, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_collect         = try_get_symbol(handle, SYMBOL_FN_REGION_COLLECT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_free            = try_get_symbol(handle, SYMBOL_FN_REGION_FREE, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_get_last_node   = try_get_symbol(handle, SYMBOL_FN_REGION_GET_LAST_NODE, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_merge           = try_get_symbol(handle, SYMBOL_FN_REGION_MERGE, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_push            = try_get_symbol(handle, SYMBOL_FN_REGION_PUSH, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_push_int        = try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_INT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_push_float      = try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_FLOAT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_push_double     = try_get_symbol(handle, SYMBOL_FN_REGION_PUSH_DOUBLE, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->region_shrink_capacity = try_get_symbol(handle, SYMBOL_FN_REGION_SHRINK_CAPACITY, &is_error); REGION_API_HANDLE_UNWRAP;

    region_api->stack_region_alloc   = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_ALLOC, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_free    = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_FREE, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_peek    = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PEEK, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_peek_at = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PEEK_AT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_swap    = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_SWAP, &is_error); REGION_API_HANDLE_UNWRAP;
    
    region_api->stack_region_push        = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_push_int    = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_INT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_push_float  = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_FLOAT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_push_double = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_PUSH_DOUBLE, &is_error); REGION_API_HANDLE_UNWRAP;
    
    region_api->stack_region_pop        = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_pop_int    = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP_INT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_pop_float  = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP_FLOAT, &is_error); REGION_API_HANDLE_UNWRAP;
    region_api->stack_region_pop_double = try_get_symbol(handle, SYMBOL_FN_STACK_REGION_POP_DOUBLE, &is_error); REGION_API_HANDLE_UNWRAP;

    region_api->test_set_available_memory = try_get_symbol(handle, SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY, &is_error); REGION_API_HANDLE_UNWRAP; 
    return region_api;

fatal:
    if (region_api) free(region_api);
    TEST_LOG_LINE(stderr, "Error", "Could not initialize the region api handle. Stop.");
    return NULL;
}

bool try_load_file_and_test(RegionAPI *api, const char *file_path)
{
    if (!api || !file_path) {
        TEST_LOG_LINE(stderr, "Error", "Could not load a file and test it: invalid arguments");
        return false;
    }

    void *handle = NULL;

    TestContext *(*get_start)(void) = NULL;
    TestContext *(*get_end)(void) = NULL;

    handle = try_load_shared_object(file_path); if (!handle) goto fatal;

    bool is_error = false;

    get_start = try_get_symbol(handle, GET_TESTS_START_STR, &is_error); if (is_error) goto fatal;
    get_end   = try_get_symbol(handle, GET_TESTS_END_STR, &is_error);   if (is_error) goto fatal;

    for (TestContext *t = get_start(); t < get_end(); t++) {
        TestResult result = t->func(api);
    
        if (result.success) {
            TEST_LOG_PASSED_TEST(t);
            continue;
        } 
        
        TEST_LOG_FAILED_TEST(t, result);
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
        TEST_LOG_LINE(stderr, "Error", "Failed to allocate memory while testing: no directory or file name");
        return NULL;   
    }

    size_t file_name_len = strlen(file_name);
    size_t directory_len = strlen(directory);

    size_t file_path_len = file_name_len + directory_len + 2;

    char *file_path = (char *)malloc(sizeof(char) * file_path_len);

    if (!file_path) {
        TEST_LOG_LINE(stderr, "Error", "Failed to allocate memory while testing");
        return NULL;
    }

    memcpy(file_path, directory, directory_len);
    memcpy(file_path + directory_len, "/", 1);
    memcpy(file_path + directory_len + 1, file_name, file_name_len);
    memcpy(file_path + directory_len + 1 + file_name_len, "\0", 1);

    return file_path;
}

bool try_load_files_and_test(const char *directory) 
{
    if (!directory) {
        TEST_LOG_LINE(stderr, "Error", "Could not load files and test: no directory");
        return false;
    }

    struct dirent *entry = NULL;

    DIR *dir = opendir(directory);

    if (!dir) {
        TEST_LOG_LINE(stderr, "Error", "Could not open a directory \"%s\": %s",
            directory, strerror(errno));
        return false;
    }

    void *region_api_handle = try_load_shared_object(TEST_REGION_TEST_SO_FILE_PATH);

    if (!region_api_handle) {
        closedir(dir);
        return false;
    }

    RegionAPI *api = try_get_region_api_handle(region_api_handle);

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

        bool test_result = try_load_file_and_test(api, file_path);

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