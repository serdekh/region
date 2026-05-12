#include "../include/rt-shared.h"

#define TEST_STACK_REGION_CAPACITY 1

TestResult test_stack_region_push_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegionFrame frame = api->stack_region_push(NULL, 1, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, frame.data);

    return result;
}

TestResult test_stack_region_push_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error);

    StackRegionFrame frame = api->stack_region_push(&stack, 0, &error);

    RT_TEST_RESULT_WRITE_INT(result, 0, (int)frame.size);

    api->stack_region_free(&stack);

    return result;
}

TestResult test_stack_region_push_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->stack_region_push(&stack, SIZE_MAX, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_ARG_LARGE_SIZE, error.message);

    api->stack_region_free(&stack);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_push_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_CAPACITY, &error); RT_TARGET_UNWRAP;

    api->test_set_available_memory(TEST_STACK_REGION_CAPACITY / 2);

        api->stack_region_push(&stack, TEST_STACK_REGION_CAPACITY * 2, &error);

    api->test_set_default_memory();

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION, error.message);

    api->stack_region_free(&stack);

    return result;

    RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));
}

RT_TEST_MODULE_REGISTER(test_stack_region_push_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_push_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_push_case_3, 3);
RT_TEST_MODULE_REGISTER(test_stack_region_push_case_4, 4);

RT_TEST_MODULE_EXPORT;