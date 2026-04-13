#include "common/common.h"

FuncPtr_stack_region_peek_at fn = NULL;

#define TEST_STACK_REGION_PEEK_AT_CASE_3_CAPACITY 1
#define TEST_STACK_REGION_PEEK_AT_CASE_3_HUGE_INDEX TEST_STACK_REGION_PEEK_AT_CASE_3_CAPACITY * 1000

#define TEST_STACK_REGION_PEEK_AT_CASE_4_ITEM_SIZE sizeof(int)
#define TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT 5
#define TEST_STACK_REGION_PEEK_AT_CASE_4_CAPACITY                                             \
    TEST_STACK_REGION_PEEK_AT_CASE_4_ITEM_SIZE * TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT \

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK_AT); 
}

TestResult test_stack_region_peek_at_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, 1, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_AT_NO_STACK_REGION, error.code);

    return result;
}

TestResult test_stack_region_peek_at_case_2()
{
    try_init_test_fn();
    
    TestResult result = {0};

    RegionError error = {0};
    StackRegion *stack = NULL;

    StackRegionFrame frame = fn(stack, 1, &error);

    TEST_RESULT_WRITE_PTR(result, NULL, frame.data);

    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if (stack) stack_region_free(&stack);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

TestResult test_stack_region_peek_at_case_3()
{
    try_init_test_fn();
    
    TestResult result = {0};

    RegionError error = {0};
    StackRegion *stack = NULL;

    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_PEEK_AT_CASE_3_CAPACITY, &error); UNWRAP;

    StackRegionFrame frame = fn(stack, TEST_STACK_REGION_PEEK_AT_CASE_3_HUGE_INDEX, &error); 

    TEST_RESULT_WRITE_PTR(result, NULL, frame.data); 

    stack_region_free(&stack);

    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if (stack) stack_region_free(&stack);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

TestResult test_stack_region_peek_at_case_4()
{
    try_init_test_fn();
    
    TestResult result = {0};

    RegionError error = {0};
    StackRegion *stack = NULL;

    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE);
    FuncPtr_stack_region_push  stack_region_push  = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_PEEK_AT_CASE_4_CAPACITY, &error); UNWRAP;

    for (int i = 0; i < TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT; i++) {
        StackRegionFrame frame = stack_region_push(stack, TEST_STACK_REGION_PEEK_AT_CASE_4_ITEM_SIZE, &error); UNWRAP;
        *(int *)frame.data = (i + 1) * 100;
    }

    for (int i = 0; i < TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT; i++) {
        StackRegionFrame frame = fn(stack, i, &error); UNWRAP;
        int expected_value = (TEST_STACK_REGION_PEEK_AT_CASE_4_ITEMS_COUNT - i) * 100;

        if (*(int *)frame.data == expected_value) continue;

        sprintf(result.expected, "Value: [%d] at: [%d]", expected_value, i);
        sprintf(result.actual,   "Value: [%d] at: [%d]", *(int *)frame.data, i);

        result.success = false;
        stack_region_free(&stack);

        return result;
    }

    result.success = true;
    stack_region_free(&stack);

    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Could not perfom a test. Stop.\n");

    if (stack) stack_region_free(&stack);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_stack_region_peek_at_case_1, 1);
REGISTER_TEST(test_stack_region_peek_at_case_2, 2);
REGISTER_TEST(test_stack_region_peek_at_case_3, 3);
REGISTER_TEST(test_stack_region_peek_at_case_4, 4);

EXPOSE(tests);