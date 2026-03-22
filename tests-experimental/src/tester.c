#include <dlfcn.h>

#include "./include/shared.h"

const TestContext *(*get_start)(void);
const TestContext *(*get_end  )(void);

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
        int result = t->func();
    
        if (result) {
            fprintf(stdout, "[Test][Fn: \"%s\"][Case: %zu]: Passed!\n", 
                t->func_name, t->case_number);
        } else {
            fprintf(stderr, "[Test][Fn: \"%s\"][Case: %zu]: Failed...(%d)\n", 
                t->func_name, t->case_number, result);
        }
    }
    
    dlclose(handle);
    return 0;
}

int main()
{
    int add_result = load_and_test("./.build/obj/add.so"); if (add_result != 0) return 1;
    int sub_result = load_and_test("./.build/obj/sub.so"); if (sub_result != 0) return 1;

    return 0;
}