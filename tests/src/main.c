#include "./tester/tester.h"

#define SO_FOLDER "./.build/obj/"

#define SO_REGION_ALLOC SO_FOLDER"__region_alloc.so"
#define SO_REGION_PUSH SO_FOLDER"__region_push.so"

int main()
{
    try_load_and_test(SO_REGION_ALLOC);
    try_load_and_test(SO_REGION_PUSH);

    printf("[Test][Passed] All tests have passed\n");

    return 0;
}