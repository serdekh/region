#include "../include/shared.h"

EXPOSE(tests);

int sub(int x, int y) 
{
    return x - y;
}

int test_sub_0() 
{
    return sub(1, 2) == -1;
}

REGISTER_TEST(test_sub_0, 1);