#include "../include/rt-shared.h"

#define REGION_TEST_REGION_CLONE_STATIC_DATA "Hello from tests!"
#define REGION_TEST_REGION_CLONE_STATIC_DATA_LENGTH sizeof(REGION_TEST_REGION_CLONE_STATIC_DATA)

TestResult test_region_clone_case_1(RegionAPI *api)
{  
    TestResult result = {0};

    RegionError error = REGION_ERROR_INIT;

    void *clone = api->region_clone(NULL, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, clone);

    return result;
}

TestResult test_region_clone_case_2(RegionAPI *api)
{
    TestResult result = {0};

    Region region     = {0};
    RegionError error = REGION_ERROR_INIT;

    api->test_set_available_memory(sizeof(Region *) / 2);

    api->region_clone(&region, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION, error.message);

    api->test_set_default_memory();

    return result;
}

TestResult test_region_clone_case_3(RegionAPI *api)
{
    TestResult  result = {0};

    Region      root   = { .capacity = 1 };
    Region      second = { .capacity = 1 };
    RegionError error = REGION_ERROR_INIT;

    root.next = &second;

    api->test_set_available_memory(sizeof(root) + root.capacity);

    api->region_clone(&root, &error); 

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION, error.message);

    api->test_set_default_memory();

    return result;
}

TestResult test_region_clone_case_4(RegionAPI *api)
{
    TestResult  result = {0};
    
    Region region = {0};
    Region *clone = NULL;
    RegionError error  = REGION_ERROR_INIT;

    size_t static_data_len = strlen(REGION_TEST_REGION_CLONE_STATIC_DATA);

    region.capacity = region.size = static_data_len;
    region.data = REGION_TEST_REGION_CLONE_STATIC_DATA;

    clone = api->region_clone(&region, &error); RT_TARGET_UNWRAP;

    memcpy(result.actual, clone->data, REGION_TEST_REGION_CLONE_STATIC_DATA_LENGTH);
    memcpy(result.expected, REGION_TEST_REGION_CLONE_STATIC_DATA, REGION_TEST_REGION_CLONE_STATIC_DATA_LENGTH);
    result.success = strcmp(clone->data, REGION_TEST_REGION_CLONE_STATIC_DATA) == 0;

    api->region_free(&clone);

    return result;

    RT_TARGET_FATAL_ERROR(if (clone) api->region_free(&clone););
}

RT_TEST_MODULE_REGISTER(test_region_clone_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_clone_case_2, 2);
RT_TEST_MODULE_REGISTER(test_region_clone_case_3, 3);
RT_TEST_MODULE_REGISTER(test_region_clone_case_4, 4);

RT_TEST_MODULE_EXPORT;
