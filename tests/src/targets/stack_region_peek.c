#include "../include/rt-shared.h"

#define TEST_STACK_REGION_PEEK_RANDOM_VALUE 42
#define TEST_STACK_REGION_PEEK_CASE_2_CAPACITY sizeof(int) + sizeof(size_t)
#define TEST_STACK_REGION_PEEK_CASE_3_CAPACITY sizeof(size_t)
#define TEST_STACK_REGION_PEEK_CASE_3_CORRUPTED_COUNT 1
#define TEST_STACK_REGION_PEEK_CASE_4_CAPACITY sizeof(int) + sizeof(size_t) + sizeof(size_t)
#define TEST_STACK_REGION_PEEK_CASE_4_CORRUPTED_COUNT 2

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
    
    stack = api->stack_region_alloc(TEST_STACK_REGION_PEEK_CASE_2_CAPACITY, &error); RT_TARGET_UNWRAP;
    
    StackRegionFrame frame = api->stack_region_push(&stack, sizeof(int), &error); RT_TARGET_UNWRAP;
    
    *(int *)frame.data = TEST_STACK_REGION_PEEK_RANDOM_VALUE;
    
    StackRegionFrame peeked_frame = api->stack_region_peek(stack, &error); RT_TARGET_UNWRAP;
  
    RT_TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_PEEK_RANDOM_VALUE, *(int *)peeked_frame.data);

    api->stack_region_free(&stack);

    return result;
    
    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_peek_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = {0};
    StackRegion *stack = api->stack_region_alloc(TEST_STACK_REGION_PEEK_CASE_3_CAPACITY, &error); RT_TARGET_UNWRAP;

    *(size_t *)(stack->data - sizeof(size_t)) = TEST_STACK_REGION_PEEK_CASE_3_CORRUPTED_COUNT;

    api->stack_region_peek(stack, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_CORRUPTED_DATA, error.code);

    api->stack_region_free(&stack);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_peek_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = {0};
    StackRegion *stack = api->stack_region_alloc(TEST_STACK_REGION_PEEK_CASE_4_CAPACITY, &error); RT_TARGET_UNWRAP;

    *(size_t *)(stack->data - sizeof(size_t)) = TEST_STACK_REGION_PEEK_CASE_4_CORRUPTED_COUNT;

    api->stack_region_peek(stack, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_CORRUPTED_DATA, error.code);

    api->stack_region_free(&stack);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

RT_TEST_MODULE_REGISTER(test_stack_region_peek_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_peek_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_peek_case_3, 3);
RT_TEST_MODULE_REGISTER(test_stack_region_peek_case_4, 4);

RT_TEST_MODULE_EXPORT;