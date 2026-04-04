#include "./common/common.h"

FuncPtr_region_get_last_node fn = NULL;

#define test_fn(region, error) fn((region), (error), REGION_GET_CURRENT_FILE_LOCATION)

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

    test_fn(NULL, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_GET_LAST_NODE_NO_REGION, error.code);

    return result;
}

#define TEST_REGION_GET_LAST_NODE_CAPACITY 1
#define UNWRAP_IF_ERROR if (REGION_ERROR(error)) goto fatal

TestResult test_region_get_last_node_case_2()
{
    try_init_test_fn();
    
    TestResult result = {0};
    RegionError error = {0};

    Region *first = NULL;
    Region *second = NULL;

    FuncPtr_region_alloc __region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
    FuncPtr_region_free region_free = try_get_symbol(SYMBOL_FN_REGION_FREE);

    first  = region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, &error); UNWRAP_IF_ERROR;
    second = region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, &error); UNWRAP_IF_ERROR;

    first->next = second;

    Region *last_node = test_fn(first, &error); UNWRAP_IF_ERROR;

    TEST_RESULT_WRITE_PTR(result, second, last_node);

    region_free(&first);
    
    return result;

fatal:
    if (first)  region_free(&first);
    if (_RegionHandle) dlclose(_RegionHandle);
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Failed to perform a test for the `region_get_last_node` function. Stop.\n");
    exit(1);
}

REGISTER_TEST(test_region_get_last_node_case_1, 1);
REGISTER_TEST(test_region_get_last_node_case_2, 2);

EXPOSE(tests);