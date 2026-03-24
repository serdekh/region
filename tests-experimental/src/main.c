#include "./tester/tester.h"

int main()
{
    int region_alloc_result = load_and_test("./.build/obj/__region_alloc.so"); 

    if (region_alloc_result != 0) return 1;

    return 0;
}