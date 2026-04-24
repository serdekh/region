#include "../include/shared.h"

#define TEST_STACK_REGION_SWAP_CASE2_CAPACITY sizeof(int)
#define TEST_STACK_REGION_SWAP_CASE2_VALUE 10

#define TEST_STACK_REGION_SWAP_CASE3_CAPACITY sizeof(int) * 2 + sizeof(size_t) * 2
#define TEST_STACK_REGION_SWAP_CASE3_VALUE1 123
#define TEST_STACK_REGION_SWAP_CASE3_VALUE2 456

#define TEST_STACK_REGION_SWAP_CASE4_CAPACITY sizeof(int) + sizeof(float) + sizeof(size_t) * 2
#define TEST_STACK_REGION_SWAP_CASE4_VALUE_INT 123
#define TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT 456.789f

TestResult test_stack_region_swap_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->stack_region_swap(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_NO_ERROR, error.code);

    return result;
}

TestResult test_stack_region_swap_case_2(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_SWAP_CASE2_CAPACITY, &error); UNWRAP;

    int *pushed = (int *)(api->stack_region_push(&stack, TEST_STACK_REGION_SWAP_CASE2_CAPACITY, &error)).data; UNWRAP;

    *pushed = TEST_STACK_REGION_SWAP_CASE2_VALUE;

    api->stack_region_swap(stack, &error);

    StackRegionFrame pushed_after_swap = api->stack_region_peek(stack, &error); UNWRAP;

    TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_SWAP_CASE2_VALUE, *(int *)pushed_after_swap.data);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

TestResult test_stack_region_swap_case_3(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_SWAP_CASE3_CAPACITY, &error); UNWRAP;

    int *item_2 = (int *)(api->stack_region_push(&stack, sizeof(int), &error)).data; UNWRAP;
    int *item_1 = (int *)(api->stack_region_push(&stack, sizeof(int), &error)).data; UNWRAP;

    *item_1 = TEST_STACK_REGION_SWAP_CASE3_VALUE1;
    *item_2 = TEST_STACK_REGION_SWAP_CASE3_VALUE2;

    api->stack_region_swap(stack, &error);

    int *swapped_item_1 = (int *)(api->stack_region_peek_at(stack, 0, &error)).data; UNWRAP;
    int *swapped_item_2 = (int *)(api->stack_region_peek_at(stack, 1, &error)).data; UNWRAP;

    sprintf(result.expected, "Before swap: {%d, %d}, After swap: {%d, %d}", 
        TEST_STACK_REGION_SWAP_CASE3_VALUE2, TEST_STACK_REGION_SWAP_CASE3_VALUE1,
        TEST_STACK_REGION_SWAP_CASE3_VALUE1, TEST_STACK_REGION_SWAP_CASE3_VALUE2);

    sprintf(result.actual, "Before swap: {%d, %d}, After swap: {%d, %d}", 
        TEST_STACK_REGION_SWAP_CASE3_VALUE2, TEST_STACK_REGION_SWAP_CASE3_VALUE1,
        *swapped_item_2, *swapped_item_1);

    result.success = (*swapped_item_2 == TEST_STACK_REGION_SWAP_CASE3_VALUE1) && 
                     (*swapped_item_1 == TEST_STACK_REGION_SWAP_CASE3_VALUE2);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

TestResult test_stack_region_swap_case_4(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_SWAP_CASE4_CAPACITY, &error); UNWRAP;

    int   *item_2 = (int *)(api->stack_region_push(&stack, sizeof(int), &error).data); UNWRAP;
    float *item_1 = (float *)(api->stack_region_push(&stack, sizeof(float), &error)).data; UNWRAP;

    *item_1 = TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT;
    *item_2 = TEST_STACK_REGION_SWAP_CASE4_VALUE_INT;

    api->stack_region_swap(stack, &error);

    int *swapped_item_1 = (int *)(api->stack_region_pop(stack, &error)).data; UNWRAP;
    float *swapped_item_2 = (float *)(api->stack_region_pop(stack, &error)).data; UNWRAP;

    sprintf(result.expected, "Before swap: {%d, %f}, After swap: {%f, %d}", 
        TEST_STACK_REGION_SWAP_CASE4_VALUE_INT, TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT,
        TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT, TEST_STACK_REGION_SWAP_CASE4_VALUE_INT);

    sprintf(result.actual, "After swap: {%f, %d}", *swapped_item_2, *swapped_item_1);

    result.success = (*swapped_item_2 == TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT) && 
                     (*swapped_item_1 == TEST_STACK_REGION_SWAP_CASE4_VALUE_INT);

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

REGISTER_TEST(test_stack_region_swap_case_1, 1);
REGISTER_TEST(test_stack_region_swap_case_2, 2);
REGISTER_TEST(test_stack_region_swap_case_3, 3);
REGISTER_TEST(test_stack_region_swap_case_4, 4);

EXPORT_AT_TESTS_SECTION