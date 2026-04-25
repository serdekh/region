#include "rt.h"

#define SO_FOLDER ".build/obj"

int main()
{
    if (!rt_try_load_files_and_test(SO_FOLDER)) {
        RT_LOG_LINE(stderr, "Log", "Not all tests have passed");
        return 1;
    }
    
    RT_LOG_PASSED_ALL_TESTS;

    return 0;
}