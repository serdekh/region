#include "../include/shared.h"

#define TEST_STACK_REGION_POP_CAPACITY sizeof(int) * 2 + sizeof(size_t) * 2
#define TEST_STACK_REGION_POP_RANDOM_VALUE 512

TestResult test_stack_region_pop_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegionFrame frame = api->stack_region_pop(NULL, &error);

    TEST_RESULT_WRITE_PTR(result, NULL, frame.data);

    return result;
}

TestResult test_stack_region_pop_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    
    StackRegion *stack = api->stack_region_alloc(TEST_STACK_REGION_POP_CAPACITY, &error); UNWRAP;

    api->stack_region_push(&stack, sizeof(int), &error); UNWRAP;
    api->stack_region_push(&stack, sizeof(int), &error); UNWRAP;
    
    api->stack_region_pop(stack, &error); UNWRAP;
    
    TEST_RESULT_WRITE_INT(result, (int)(sizeof(int) + sizeof(size_t)), (int)stack->size);
    
    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

REGISTER_TEST(test_stack_region_pop_case_1, 1);
REGISTER_TEST(test_stack_region_pop_case_2, 2);

EXPORT_AT_TESTS_SECTION