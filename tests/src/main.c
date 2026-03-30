#include "./tester/tester.h"

#define SO_FOLDER ".build/obj"

int main()
{
    try_load_files_and_test(SO_FOLDER);
    
    printf("[Test][Passed] All tests have passed\n");

    return 0;
}