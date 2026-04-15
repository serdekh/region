#include "./common/common.h"

#define TEST_REGION_GET_LAST_NODE_CAPACITY 1

TestResult test_region_get_last_node_case_1()
{
    RegionAPI *api = try_get_region_api_handle();

    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    api->region_get_last_node(NULL, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error);

    TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_GET_LAST_NODE_NO_REGION, error.code);

    return result;
}

Region *_alloc_region_with_two_nodes(RegionError *error)
{
    RegionAPI *api = try_get_region_api_handle();

    Region *first = NULL;
    Region *second = NULL;

    first = api->region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, error); 

    if (!first) return NULL;

    second = api->region_alloc(TEST_REGION_GET_LAST_NODE_CAPACITY, error); 

    if (!second) {
        api->region_free(&first);
        return NULL;
    }

    first->next = second;

    return first;
}

TestResult test_region_get_last_node_case_2()
{
    RegionAPI *api = try_get_region_api_handle();
    
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region_with_two_nodes = _alloc_region_with_two_nodes(&error); UNWRAP;

    Region *last_node = api->region_get_last_node(region_with_two_nodes, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error); UNWRAP;

    TEST_RESULT_WRITE_PTR(result, region_with_two_nodes->next, last_node);

    api->region_free(&region_with_two_nodes);
    
    return result;

    TEST_FATAL(
        if (region_with_two_nodes) api->region_free(&region_with_two_nodes);
        close_region_api_handle();
    );
}

TestResult test_region_get_last_node_case_3()
{
    RegionAPI *api = try_get_region_api_handle();
    
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region_with_two_nodes = _alloc_region_with_two_nodes(&error); UNWRAP;

    region_with_two_nodes->next->size = 0;

    Region *last_node = api->region_get_last_node(region_with_two_nodes, REGION_GET_LAST_NODE_OPTION_NON_EMPTY, &error); UNWRAP;

    TEST_RESULT_WRITE_PTR(result, region_with_two_nodes, last_node);

    api->region_free(&region_with_two_nodes);
    
    return result;

    TEST_FATAL(
        if (region_with_two_nodes) api->region_free(&region_with_two_nodes);
        close_region_api_handle();
    );
}

REGISTER_TEST(test_region_get_last_node_case_1, 1);
REGISTER_TEST(test_region_get_last_node_case_2, 2);
REGISTER_TEST(test_region_get_last_node_case_3, 3);

EXPOSE(tests);