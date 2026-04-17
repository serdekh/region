#include "./tester/tester.h"

#define SO_FOLDER ".build/obj/"

int main()
{
    if (!try_load_files_and_test(SO_FOLDER)) {
        TEST_LOG_LINE(stderr, "Log", "Not all tests have passed");
        return 1;
    }
    
    TEST_LOG_PASSED_ALL_TESTS;

    return 0;
}