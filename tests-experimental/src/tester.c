#include <dlfcn.h>

#include "../../src/region.h"
#include "./include/shared.h"

const TestContext *(*get_start)(void);
const TestContext *(*get_end  )(void);

// TODO: Extract this function into a separate file which can
// be used by both the functions and the framework
int load_and_test(const char *file_path)
{
    void *handle = dlopen(file_path, RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "[Test][Error]: Could not load a dynamic executable (%s): %s\n",
            file_path, dlerror());
        return 1;
    }
    
    get_start = dlsym(handle, "get_tests_start");
    get_end   = dlsym(handle, "get_tests_end");
    
    for (const TestContext *t = get_start(); t < get_end(); ++t) {
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
    return 0;
}

int main()
{
    int region_alloc_result = load_and_test("./.build/obj/__region_alloc.so"); 

    if (region_alloc_result != 0) return 1;

    return 0;
}