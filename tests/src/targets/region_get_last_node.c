#include "../include/rt-shared.h"

#define TEST_REGION_GET_LAST_NODE_CAPACITY 1

TestResult test_region_get_last_node_case_1(RegionAPI *api)
{
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *last_node = api->region_get_last_node(NULL, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, last_node);

    return result;
}

Region *_alloc_region_with_two_nodes(RegionAPI *api, RegionError *error)
{
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

TestResult test_region_get_last_node_case_2(RegionAPI *api)
{   
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region_with_two_nodes = _alloc_region_with_two_nodes(api, &error); RT_TARGET_UNWRAP;

    Region *last_node = api->region_get_last_node(region_with_two_nodes, REGION_GET_LAST_NODE_OPTION_DEFAULT, &error); RT_TARGET_UNWRAP;

    RT_TEST_RESULT_WRITE_PTR(result, region_with_two_nodes->next, last_node);

    api->region_free(&region_with_two_nodes);
    
    return result;

    RT_TARGET_FATAL_ERROR(
        if (region_with_two_nodes) api->region_free(&region_with_two_nodes)
    );
}

TestResult test_region_get_last_node_case_3(RegionAPI *api)
{   
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    Region *region_with_two_nodes = _alloc_region_with_two_nodes(api, &error); RT_TARGET_UNWRAP;

    region_with_two_nodes->next->size = 0;

    Region *last_node = api->region_get_last_node(region_with_two_nodes, REGION_GET_LAST_NODE_OPTION_NON_EMPTY, &error); RT_TARGET_UNWRAP;

    RT_TEST_RESULT_WRITE_PTR(result, region_with_two_nodes, last_node);

    api->region_free(&region_with_two_nodes);
    
    return result;

    RT_TARGET_FATAL_ERROR(
        if (region_with_two_nodes) api->region_free(&region_with_two_nodes);
    );
}

RT_TEST_MODULE_REGISTER(test_region_get_last_node_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_get_last_node_case_2, 2);
RT_TEST_MODULE_REGISTER(test_region_get_last_node_case_3, 3);

RT_TEST_MODULE_EXPORT;