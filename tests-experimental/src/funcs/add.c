#include "../include/shared.h"

EXPOSE(tests);

int add(int x, int y) 
{
    return x + y;
}

int test_add_0() 
{
    return add(1, 2) == 3;
}

REGISTER_TEST(test_add_0, 1);