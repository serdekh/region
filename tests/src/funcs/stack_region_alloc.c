#include "../include/shared.h"

#define TEST_STACK_REGION_ALLOC_CAPACITY 1

TestResult test_stack_region_alloc_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = api->stack_region_alloc(0, &error); UNWRAP;

    result.success = stack->capacity == STACK_REGION_CACHE_COUNT_SIZE && stack->data != NULL;

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_alloc_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->stack_region_alloc(SIZE_MAX, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY, error.code);

    return result;
}


TestResult test_stack_region_alloc_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(StackRegion) / 2);

    api->stack_region_alloc(TEST_STACK_REGION_ALLOC_CAPACITY, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_REGION, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

TestResult test_stack_region_alloc_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(StackRegion));

    api->stack_region_alloc(TEST_STACK_REGION_ALLOC_CAPACITY, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_CAPACITY, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    return result;
}

REGISTER_TEST(test_stack_region_alloc_case_1, 1);
REGISTER_TEST(test_stack_region_alloc_case_2, 2);
REGISTER_TEST(test_stack_region_alloc_case_3, 3);
REGISTER_TEST(test_stack_region_alloc_case_4, 4);

EXPORT_AT_TESTS_SECTION