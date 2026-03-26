#include "./tester.h"

void *try_load_shared_object(const char *file_path)
{
    void *handle = dlopen(file_path, RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "[Test][Error]: Could not load a dynamic executable (%s): %s\nStop.\n",
            file_path, dlerror());
        exit(1);
    }

    return handle;
}

void try_load_and_test(const char *file_path)
{
    void *handle = try_load_shared_object(file_path);

    const TestContext *(*get_start)(void) = dlsym(handle, GET_TESTS_START_STR);
    const TestContext *(*get_end  )(void) = dlsym(handle, GET_TESTS_END_STR);

    for (const TestContext *t = get_start(); t < get_end(); t++) {
        TestResult result = t->func();
    
        if (result.success) {
            fprintf(stdout, "[Test][Fn: \"%s\"][Case: %zu]: Passed!\n", 
                t->func_name, t->case_number);
            continue;
        } 
        
        fprintf(stderr, "[Test][Fn: \"%s\"][Case: %zu]: Failed\n\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n", 
            t->func_name, t->case_number, result.expected, result.actual);
        
        exit(1);    
    }
    
    dlclose(handle);
}