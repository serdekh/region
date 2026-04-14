#include "./common/common.h"

FuncPtr_region_get_last_node fn = NULL;

FuncPtr_region_free  lib_region_free   = NULL;
FuncPtr_region_alloc lib_region_alloc  = NULL;

#define TEST_REGION_GET_LAST_NODE_CAPACITY 1

void try_init_test_fn()
{
    if (!_RegionHandle) try_get_region_handle();

    if (!fn) fn = try_get_symbol(SYMBOL_FN_REGION_GET_LAST_NODE);

    if (!lib_region_free)  lib_region_free  = try_get_symbol(SYMBOL_FN_REGION_FREE);
    if (!lib_region_alloc) lib_region_alloc = try_get_symbol(SYMBOL_FN_REGION_ALLOC);
}

TestResult test_region_get_last_node_case_1()
{
    try_init_test_fn();

    TestResult result = {0};
    RegionError error = {0}; REGION_ERROR_INIT_LOCATION(&error);

    fn(NULL, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_GET_LAST_NODE_NO_REGION, error.code);

    return result;
}

Region *_alloc_region_with_two_nodes(RegionError *error)
{
    Region *first = NULL;
    Region *second = NULL;

    first = lib_region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, error); 

    if (!first) return NULL;

    second = lib_region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, error); 

    if (!second) {
        lib_region_free(&first);
        return NULL;
    }

    first->next = second;

    return first;
}

TestResult test_region_get_last_node_case_2()
{
    try_init_test_fn();
    
    TestResult result = {0};
    RegionError error = {0}; REGION_ERROR_INIT_LOCATION(&error);

    Region *region_with_two_nodes = _alloc_region_with_two_nodes(&error); UNWRAP;

    Region *last_node = fn(region_with_two_nodes, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error); UNWRAP;

    TEST_RESULT_WRITE_PTR(result, region_with_two_nodes->next, last_node);

    lib_region_free(&region_with_two_nodes);
    
    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Failed to perform a test for the `region_get_last_node` function. Stop.\n");

    if (region_with_two_nodes) lib_region_free(&region_with_two_nodes);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

TestResult test_region_get_last_node_case_3()
{
    try_init_test_fn();
    
    TestResult result = {0};
    RegionError error = {0}; REGION_ERROR_INIT_LOCATION(&error);

    Region *region_with_two_nodes = _alloc_region_with_two_nodes(&error); UNWRAP;

    region_with_two_nodes->next->size = 0;

    Region *last_node = fn(region_with_two_nodes, REGION_GET_LAST_NODE_OPTION_NON_EMPTY, &error); UNWRAP;

    TEST_RESULT_WRITE_PTR(result, region_with_two_nodes, last_node);

    lib_region_free(&region_with_two_nodes);
    
    return result;

fatal:
    REGION_LOG_ERROR(error);
    fprintf(stderr, "[Test][Error]: Failed to perform a test for the `region_get_last_node` function. Stop.\n");

    if (region_with_two_nodes) lib_region_free(&region_with_two_nodes);
    if (_RegionHandle) dlclose(_RegionHandle);

    exit(1);
}

REGISTER_TEST(test_region_get_last_node_case_1, 1);
REGISTER_TEST(test_region_get_last_node_case_2, 2);
REGISTER_TEST(test_region_get_last_node_case_3, 3);

EXPOSE(tests);