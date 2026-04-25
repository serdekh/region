#include "../include/rt-shared.h"

#define REGION_TEST_REGION_CLONE_STATIC_DATA "Hello from tests!"

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

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_ROOT, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

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

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_NODE, error.code);

    api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);

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

    memcpy(result.actual, clone->data, RT_TEST_RESULT_MAX_STRING_SIZE);
    memcpy(result.expected, REGION_TEST_REGION_CLONE_STATIC_DATA, RT_TEST_RESULT_MAX_STRING_SIZE);
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