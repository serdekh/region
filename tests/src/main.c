#include "./tester/tester.h"

#define SO_FOLDER "./.build/obj/"

#define SO_REGION_ALLOC SO_FOLDER"__region_alloc.so"
#define SO_REGION_PUSH SO_FOLDER"__region_push.so"
#define SO_REGION_SHRINK_CAPACITY SO_FOLDER"__region_shrink_capacity.so"
#define SO_STACK_REGION_ALLOC SO_FOLDER"__stack_region_alloc.so"

int main()
{
    try_load_and_test(SO_REGION_ALLOC);
    try_load_and_test(SO_REGION_PUSH);
    try_load_and_test(SO_REGION_SHRINK_CAPACITY);
    try_load_and_test(SO_STACK_REGION_ALLOC);

    printf("[Test][Passed] All tests have passed\n");

    return 0;
}