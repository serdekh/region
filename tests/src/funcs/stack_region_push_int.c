#include "../include/shared.h"

#define TEST_STACK_REGION_PUSH_INT_CASE2_STATIC_DATA "TEST_STACK_REGION_PUSH_INT_CASE2_STATIC_DATA"
#define TEST_STACK_REGION_PUSH_INT_CASE2_STATIC_DATA_LENGTH sizeof(TEST_STACK_REGION_PUSH_INT_CASE2_STATIC_DATA)

#define TEST_STACK_REGION_PUSH_INT_CASE3_VALUE 1
#define TEST_STACK_REGION_PUSH_INT_CASE4_VALUE 2

TestResult test_stack_region_push_int_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->stack_region_push_int(NULL, 0, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_INT_NO_STACK_REGION, error.code);

    return result;
}

TestResult test_stack_region_push_int_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion stack = {
        .next     = NULL,
        .data     = TEST_STACK_REGION_PUSH_INT_CASE2_STATIC_DATA,
        .capacity = TEST_STACK_REGION_PUSH_INT_CASE2_STATIC_DATA_LENGTH,
        .size     = TEST_STACK_REGION_PUSH_INT_CASE2_STATIC_DATA_LENGTH
    };

    api->test_set_available_memory(0);
        api->stack_region_push_int(&stack, 0, &error);
    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_INT_MALLOC_REGION, error.code);

    return result;
}

TestResult test_stack_region_push_int_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(sizeof(int) + sizeof(size_t), &error); UNWRAP;

    int *pushed = api->stack_region_push_int(stack, TEST_STACK_REGION_PUSH_INT_CASE3_VALUE, &error);

    int *peeked = (int *)(stack->data);

    if (pushed != peeked) {
        TEST_RESULT_WRITE_PTR(result, pushed, peeked);
        api->stack_region_free(&stack);
        return result;
    }

    TEST_RESULT_WRITE_INT(result, *pushed, *peeked);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack));
}

TestResult test_stack_region_push_int_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(sizeof(int) / 2, &error); UNWRAP;

    int *pushed = api->stack_region_push_int(stack, TEST_STACK_REGION_PUSH_INT_CASE4_VALUE, &error);

    int *peeked = (int *)(stack->next->data);

    if (pushed != peeked) {
        TEST_RESULT_WRITE_PTR(result, pushed, peeked);
        api->stack_region_free(&stack);
        return result;
    }

    TEST_RESULT_WRITE_INT(result, *pushed, *peeked);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack));
}

REGISTER_TEST(test_stack_region_push_int_case_1, 1);
REGISTER_TEST(test_stack_region_push_int_case_2, 2);
REGISTER_TEST(test_stack_region_push_int_case_3, 3);
REGISTER_TEST(test_stack_region_push_int_case_4, 4);

EXPORT_AT_TESTS_SECTION;