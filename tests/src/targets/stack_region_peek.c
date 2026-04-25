#include "../include/rt-shared.h"

#define TEST_STACK_REGION_PEEK_CAPACITY sizeof(int) + sizeof(size_t)
#define TEST_STACK_REGION_PEEK_RANDOM_VALUE 42

TestResult test_stack_region_peek_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegionFrame frame = api->stack_region_peek(NULL, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, frame.data);

    return result;
}

TestResult test_stack_region_peek_case_2(RegionAPI *api)
{
    TestResult result = {0};
    
    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;
    
    stack = api->stack_region_alloc(TEST_STACK_REGION_PEEK_CAPACITY, &error); RT_TARGET_UNWRAP;
    
    StackRegionFrame frame = api->stack_region_push(&stack, sizeof(int), &error); RT_TARGET_UNWRAP;
    
    *(int *)frame.data = TEST_STACK_REGION_PEEK_RANDOM_VALUE;
    
    StackRegionFrame peeked_frame = api->stack_region_peek(stack, &error); RT_TARGET_UNWRAP;
  
    RT_TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_PEEK_RANDOM_VALUE, *(int *)peeked_frame.data);

    api->stack_region_free(&stack);

    return result;
    
    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

RT_TEST_MODULE_REGISTER(test_stack_region_peek_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_peek_case_2, 2);

RT_TEST_MODULE_EXPORT;