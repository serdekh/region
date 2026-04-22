#include "../include/shared.h"

TestResult test_stack_region_push_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->stack_region_push(NULL, 1, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_NO_STACK_REGION, error.code);

    return result;
}

#define TEST_STACK_REGION_CAPACITY 1

TestResult test_stack_region_push_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); UNWRAP;

    api->stack_region_push(stack, 0, &error);

    TEST_RESULT_WRITE_INT(result, 0, (int)stack->size);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_push_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); UNWRAP;

    api->stack_region_push(stack, SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_LARGE_SIZE, error.code);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

TestResult test_stack_region_push_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); UNWRAP;

    api->test_set_available_memory(TEST_STACK_REGION_CAPACITY / 2);

    api->stack_region_push(stack, TEST_STACK_REGION_CAPACITY * 2, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

REGISTER_TEST(test_stack_region_push_case_1, 1);
REGISTER_TEST(test_stack_region_push_case_2, 2);
REGISTER_TEST(test_stack_region_push_case_3, 3);
REGISTER_TEST(test_stack_region_push_case_4, 4);

EXPORT_AT_TESTS_SECTION