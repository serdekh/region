#include "common/common.h"

FuncPtr_stack_region_swap fn = NULL;

#define TEST_STACK_REGION_SWAP_CASE2_CAPACITY sizeof(int)
#define TEST_STACK_REGION_SWAP_CASE2_VALUE 10

#define TEST_STACK_REGION_SWAP_CASE3_CAPACITY sizeof(int) * 2 + sizeof(size_t) * 2
#define TEST_STACK_REGION_SWAP_CASE3_VALUE1 123
#define TEST_STACK_REGION_SWAP_CASE3_VALUE2 456

#define TEST_STACK_REGION_SWAP_CASE4_CAPACITY sizeof(int) + sizeof(float) + sizeof(size_t) * 2
#define TEST_STACK_REGION_SWAP_CASE4_VALUE_INT 123
#define TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT 456.789f

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_STACK_REGION_SWAP);
}

TestResult test_stack_region_swap_case_1()
{
    try_init_test_fn();

    TestResult result = {0};

    RegionError error = {0};

    fn(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_STACK_REGION_SWAP_NO_STACK_REGION, error.code);

    return result;
}

TestResult test_stack_region_swap_case_2()
{
    try_init_test_fn();

    TestResult result = {0};

    RegionError error = {0}; REGION_ERROR_INIT_LOCATION(&error);

    StackRegion *stack = NULL;

    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE); 
    FuncPtr_stack_region_push  stack_region_push  = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    FuncPtr_stack_region_peek  stack_region_peek  = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK); 
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_SWAP_CASE2_CAPACITY, &error); UNWRAP;

    int *pushed = (int *)(stack_region_push(stack, TEST_STACK_REGION_SWAP_CASE2_CAPACITY, &error)).data; UNWRAP;

    *pushed = TEST_STACK_REGION_SWAP_CASE2_VALUE;

    fn(stack, &error);

    StackRegionFrame pushed_after_swap = stack_region_peek(stack, &error); UNWRAP;

    TEST_RESULT_WRITE_INT(result, TEST_STACK_REGION_SWAP_CASE2_VALUE, *(int *)pushed_after_swap.data);

    stack_region_free(&stack);

    return result;

    TEST_FATAL(
        if (stack) stack_region_free(&stack);
        if (_RegionHandle) dlclose(_RegionHandle);
    );
}

TestResult test_stack_region_swap_case_3()
{
    try_init_test_fn();

    TestResult result = {0};

    RegionError error = {0}; REGION_ERROR_INIT_LOCATION(&error);

    StackRegion *stack = NULL;

    FuncPtr_stack_region_free    stack_region_free    = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE); 
    FuncPtr_stack_region_push    stack_region_push    = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    FuncPtr_stack_region_alloc   stack_region_alloc   = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);
    FuncPtr_stack_region_peek_at stack_region_peek_at = try_get_symbol(SYMBOL_FN_STACK_REGION_PEEK_AT); 

    stack = stack_region_alloc(TEST_STACK_REGION_SWAP_CASE3_CAPACITY, &error); UNWRAP;

    int *item_2 = (int *)(stack_region_push(stack, sizeof(int), &error)).data; UNWRAP;
    int *item_1 = (int *)(stack_region_push(stack, sizeof(int), &error)).data; UNWRAP;

    *item_1 = TEST_STACK_REGION_SWAP_CASE3_VALUE1;
    *item_2 = TEST_STACK_REGION_SWAP_CASE3_VALUE2;

    fn(stack, &error);

    int *swapped_item_1 = (int *)(stack_region_peek_at(stack, 0, &error)).data; UNWRAP;
    int *swapped_item_2 = (int *)(stack_region_peek_at(stack, 1, &error)).data; UNWRAP;

    sprintf(result.expected, "Before swap: {%d, %d}, After swap: {%d, %d}", 
        TEST_STACK_REGION_SWAP_CASE3_VALUE2, TEST_STACK_REGION_SWAP_CASE3_VALUE1,
        TEST_STACK_REGION_SWAP_CASE3_VALUE1, TEST_STACK_REGION_SWAP_CASE3_VALUE2);

    sprintf(result.actual, "Before swap: {%d, %d}, After swap: {%d, %d}", 
        TEST_STACK_REGION_SWAP_CASE3_VALUE2, TEST_STACK_REGION_SWAP_CASE3_VALUE1,
        *swapped_item_2, *swapped_item_1);

    result.success = (*swapped_item_2 == TEST_STACK_REGION_SWAP_CASE3_VALUE1) && 
                     (*swapped_item_1 == TEST_STACK_REGION_SWAP_CASE3_VALUE2);

    stack_region_free(&stack);

    return result;

    TEST_FATAL(
        if (stack) stack_region_free(&stack);
        if (_RegionHandle) dlclose(_RegionHandle);
    );
}

TestResult test_stack_region_swap_case_4()
{
    try_init_test_fn();

    TestResult result = {0};

    RegionError error = {0}; REGION_ERROR_INIT_LOCATION(&error);

    StackRegion *stack = NULL;

    FuncPtr_stack_region_pop   stack_region_pop   = try_get_symbol(SYMBOL_FN_STACK_REGION_POP);
    FuncPtr_stack_region_free  stack_region_free  = try_get_symbol(SYMBOL_FN_STACK_REGION_FREE); 
    FuncPtr_stack_region_push  stack_region_push  = try_get_symbol(SYMBOL_FN_STACK_REGION_PUSH);
    FuncPtr_stack_region_alloc stack_region_alloc = try_get_symbol(SYMBOL_FN_STACK_REGION_ALLOC);

    stack = stack_region_alloc(TEST_STACK_REGION_SWAP_CASE4_CAPACITY, &error); UNWRAP;

    int   *item_2 = (int *)(stack_region_push(stack, sizeof(int), &error).data); UNWRAP;
    float *item_1 = (float *)(stack_region_push(stack, sizeof(float), &error)).data; UNWRAP;

    *item_1 = TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT;
    *item_2 = TEST_STACK_REGION_SWAP_CASE4_VALUE_INT;

    fn(stack, &error);

    int *swapped_item_1 = (int *)(stack_region_pop(stack, &error)).data; UNWRAP;
    float *swapped_item_2 = (float *)(stack_region_pop(stack, &error)).data; UNWRAP;

    sprintf(result.expected, "Before swap: {%d, %f}, After swap: {%f, %d}", 
        TEST_STACK_REGION_SWAP_CASE4_VALUE_INT, TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT,
        TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT, TEST_STACK_REGION_SWAP_CASE4_VALUE_INT);

    sprintf(result.actual, "After swap: {%f, %d}", *swapped_item_2, *swapped_item_1);

    result.success = (*swapped_item_2 == TEST_STACK_REGION_SWAP_CASE4_VALUE_FLOAT) && 
                     (*swapped_item_1 == TEST_STACK_REGION_SWAP_CASE4_VALUE_INT);

    stack_region_free(&stack);

    return result;

    TEST_FATAL(
        if (stack) stack_region_free(&stack);
        if (_RegionHandle) dlclose(_RegionHandle);
    );
}

REGISTER_TEST(test_stack_region_swap_case_1, 1);
REGISTER_TEST(test_stack_region_swap_case_2, 2);
REGISTER_TEST(test_stack_region_swap_case_3, 3);
REGISTER_TEST(test_stack_region_swap_case_4, 4);

EXPOSE(tests);