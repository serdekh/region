#include "./tester/tester.h"

#define SO_FOLDER "./.build/obj/"

#define SO_REGION_ALLOC SO_FOLDER"__region_alloc.so"
#define SO_REGION_PUSH SO_FOLDER"__region_push.so"
#define SO_REGION_SHRINK_CAPACITY SO_FOLDER"__region_shrink_capacity.so"

int main()
{
    try_load_and_test(SO_REGION_ALLOC);
    try_load_and_test(SO_REGION_PUSH);
    try_load_and_test(SO_REGION_SHRINK_CAPACITY);

    printf("[Test][Passed] All tests have passed\n");

    return 0;
}