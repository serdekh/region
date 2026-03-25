#include "./tester/tester.h"

#define SO_FOLDER "./.build/obj/"

#define SO_REGION_ALLOC SO_FOLDER"__region_alloc.so"
#define SO_REGION_ALLOC_ITEM SO_FOLDER"__region_alloc_item.so"

int main()
{
    bool region_alloc_result = load_and_test(SO_REGION_ALLOC);           if (!region_alloc_result) goto fatal;
    bool region_alloc_item_result = load_and_test(SO_REGION_ALLOC_ITEM); if (!region_alloc_item_result) goto fatal;

    fprintf(stdout, "[Test][Passed] All tests have passed\n");
    return 0;

fatal:
    fprintf(stderr, "[Test][Failed]: Not all functions have passed the tests\n");
    return 1;
}