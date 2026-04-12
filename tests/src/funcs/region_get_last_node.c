#include "./common/common.h"

FuncPtr_region_get_last_node fn = NULL;

#define TEST_REGION_GET_LAST_NODE_CAPACITY 1

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();
    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_GET_LAST_NODE);
}

TestResult test_region_get_last_node_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0};

    fn(NULL, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_GET_LAST_NODE_NO_REGION, error.code);

    return result;
}

TestResult test_region_get_last_node_case_2()
{
    try_init_test_fn();
    
    TestResult result = {0};
    RegionError error = {0};

    Region *first = NULL;
    Region *second = NULL;

    FuncPtr_region_free  region_free   = try_get_symbol(SYMBOL_FN_REGION_FREE);
    FuncPtr_region_alloc region_alloc  = try_get_symbol(SYMBOL_FN_REGION_ALLOC);

    first  = region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, &error); UNWRAP;
    second = region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, &error); UNWRAP;

    first->next = second;

    Region *last_node = fn(first, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error); UNWRAP;

    TEST_RESULT_WRITE_PTR(result, second, last_node);

    region_free(&first);
    
    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Failed to perform a test for the `region_get_last_node` function. Stop.\n");

    if (first) region_free(&first);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_region_get_last_node_case_1, 1);
REGISTER_TEST(test_region_get_last_node_case_2, 2);

EXPOSE(tests);