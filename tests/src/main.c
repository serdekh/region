#include "./tester/tester.h"

#define SO_FOLDER ".build/obj"

int main()
{
    try_load_files_and_test(SO_FOLDER);
    
    TEST_LOG_PASSED_ALL_TESTS;

    return 0;
}