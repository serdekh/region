#include "../include/rt-shared.h"

TestResult test_region_collect_case_1(RegionAPI *api)
{
    TestResult result = {0};

    size_t collected_size = 0;
    RegionError error = REGION_ERROR_INIT;

    Region **collection = api->region_collect(NULL, &collected_size, &error);

    RT_TEST_RESULT_WRITE_PTR(result, NULL, collection);

    return result;
}

TestResult test_region_collect_case_2(RegionAPI *api)
{ 
    TestResult result = {0};

    Region region = {0};
    RegionError error = REGION_ERROR_INIT;

    api->region_collect(&region, NULL, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_COLLECTED_SIZE, error.code);

    return result;
}

TestResult test_region_collect_case_3(RegionAPI *api)
{   
    TestResult result = {0};

    Region region = {0};
    RegionError error = REGION_ERROR_INIT;
    size_t collected_size = {0};

    api->test_set_available_memory(0);

    api->region_collect(&region, &collected_size, &error);

    RT_TEST_RESULT_WRITE_INT(result, REGION_ERROR_CODE_ENOMEM_REGION_COLLECT_MALLOC_COLLECTION, error.code);

    api->test_set_default_memory();

    return result;
}

RT_TEST_MODULE_REGISTER(test_region_collect_case_1, 1);
RT_TEST_MODULE_REGISTER(test_region_collect_case_2, 2);
RT_TEST_MODULE_REGISTER(test_region_collect_case_3, 3);

RT_TEST_MODULE_EXPORT;