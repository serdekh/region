#include "../include/shared.h"

#define TEST_STACK_REGION_PEEK_AT_CASE_3_CAPACITY 1
#define TEST_STACK_REGION_PEEK_AT_CASE_3_HUGE_INDEX TEST_STACK_REGION_PEEK_AT_CASE_3_CAPACITY * 1000

#define TEST_STACK_REGION_PEEK_AT_CASE_4_ITEM_SIZE sizeof(int)
#define TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT 5
#define TEST_STACK_REGION_PEEK_AT_CASE_4_CAPACITY                                             \
    TEST_STACK_REGION_PEEK_AT_CASE_4_ITEM_SIZE * TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT \

TestResult test_stack_region_peek_at_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->stack_region_peek_at(NULL, 1, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_AT_NO_STACK_REGION, error.code);

    return result;
}

TestResult test_stack_region_peek_at_case_2(RegionAPI *api)
{   
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    StackRegionFrame frame = api->stack_region_peek_at(stack, 1, &error);

    TEST_RESULT_WRITE_PTR(result, NULL, frame.data);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

TestResult test_stack_region_peek_at_case_3(RegionAPI *api)
{  
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_PEEK_AT_CASE_3_CAPACITY, &error); UNWRAP;

    StackRegionFrame frame = api->stack_region_peek_at(stack, TEST_STACK_REGION_PEEK_AT_CASE_3_HUGE_INDEX, &error); 

    TEST_RESULT_WRITE_PTR(result, NULL, frame.data); 

    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

TestResult test_stack_region_peek_at_case_4(RegionAPI *api)
{   
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;
    StackRegion *stack = NULL;

    stack = api->stack_region_alloc(TEST_STACK_REGION_PEEK_AT_CASE_4_CAPACITY, &error); UNWRAP;

    for (int i = 0; i < TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT; i++) {
        StackRegionFrame frame = api->stack_region_push(stack, TEST_STACK_REGION_PEEK_AT_CASE_4_ITEM_SIZE, &error); UNWRAP;
        *(int *)frame.data = (i + 1) * 100;
    }

    for (int i = 0; i < TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT; i++) {
        StackRegionFrame frame = api->stack_region_peek_at(stack, i, &error); UNWRAP;
        int expected_value = (TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT - i) * 100;

        if (*(int *)frame.data == expected_value) continue;

        sprintf(result.expected, "Value: [%d] at: [%d]", expected_value, i);
        sprintf(result.actual,   "Value: [%d] at: [%d]", *(int *)frame.data, i);

        result.success = false;
        api->stack_region_free(&stack);

        return result;
    }

    result.success = true;
    api->stack_region_free(&stack);

    return result;

    TEST_FATAL(if (stack) api->stack_region_free(&stack););
}

REGISTER_TEST(test_stack_region_peek_at_case_1, 1);
REGISTER_TEST(test_stack_region_peek_at_case_2, 2);
REGISTER_TEST(test_stack_region_peek_at_case_3, 3);
REGISTER_TEST(test_stack_region_peek_at_case_4, 4);

EXPORT_AT_TESTS_SECTION