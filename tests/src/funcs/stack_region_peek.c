#include "../include/shared.h"

#define TEST_STACK_REGION_PEEK_CAPACITY sizeof(int) + sizeof(size_t)
#define TEST_STACK_REGION_PEEK_RANDOM_VALUE 42

TestResult test_stack_region_peek_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->stack_region_peek(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_NO_STACK_REGION, error.code);

    return result;
}

TestResult test_stack_region_peek_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_PEEK_CAPACITY, &error); UNWRAP;

    StackRegionFrame frame = api->stack_region_push(stack, sizeof(int), &error); UNWRAP;
    
    *(int *)frame.data = TEST_STACK_REGION_PEEK_RANDOM_VALUE;
    
    StackRegionFrame peeked_frame = api->stack_region_peek(stack, &error); UNWRAP;
  
    TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_PEEK_RANDOM_VALUE, *(int *)peeked_frame.data);

    api->stack_region_free(&stack);

    return result;
    
    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

REGISTER_TEST(test_stack_region_peek_case_1, 1);
REGISTER_TEST(test_stack_region_peek_case_2, 2);

EXPORT_AT_TESTS_SECTION