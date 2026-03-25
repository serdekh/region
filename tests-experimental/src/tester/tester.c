#include "./tester.h"

bool load_and_test(const char *file_path)
{
    void *handle = dlopen(file_path, RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "[Test][Error]: Could not load a dynamic executable (%s): %s\n",
            file_path, dlerror());
        return false;
    }
    
    const TestContext *(*get_start)(void) = dlsym(handle, "get_tests_start");
    const TestContext *(*get_end  )(void) = dlsym(handle, "get_tests_end");

    for (const TestContext *t = get_start(); t < get_end(); t++) {
        bool result = t->func();
    
        if (result) {
            fprintf(stdout, "[Test][Fn: \"%s\"][Case: %zu]: Passed!\n", 
                t->func_name, t->case_number);
        } else {
            fprintf(stderr, "[Test][Fn: \"%s\"][Case: %zu]: Failed...(%d)\n", 
                t->func_name, t->case_number, result);
            dlclose(handle);
            exit(1);
        }
    }
    
    dlclose(handle);
    return true;
}