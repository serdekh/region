#include "../include/shared.h"

#define TEST_REGION_PUSH_TYPE_CASE_1_VALUE 39985.0
#define TEST_REGION_PUSH_TYPE_CASE_2_VALUE 2045.0

#define region_push_type(type, region, value, error) region_push_##type(region, value, error)

#define TEST_REGION_PUSH_TYPE_CASE_1(type, fmt)                                                      \
    TestResult test_region_push_##type##_case_1(RegionAPI *api)                                      \
    {                                                                                                \
        TestResult result = {0};                                                                     \
                                                                                                     \
        RegionError error = REGION_ERROR_INIT;                                                       \
        Region *region = NULL;                                                                       \
                                                                                                     \
        region = api->region_alloc(sizeof(type), &error); UNWRAP;                                    \
                                                                                                     \
        api->region_push_type(type, &region, TEST_REGION_PUSH_TYPE_CASE_1_VALUE, &error);            \
                                                                                                     \
        type *actual = (type *)(region->data + region->size - sizeof(type));                         \
                                                                                                     \
        TEST_RESULT_WRITE_FMT(result, fmt, (type)TEST_REGION_PUSH_TYPE_CASE_1_VALUE, (type)*actual); \
                                                                                                     \
        api->region_free(&region);                                                                   \
                                                                                                     \
        return result;                                                                               \
                                                                                                     \
        TEST_FATAL(if (region) api->region_free(&region));                                           \
    }                                                                                                \

#define TEST_REGION_PUSH_TYPE_CASE_2(type, fmt)                                                      \
    TestResult test_region_push_##type##_case_2(RegionAPI *api)                                      \
    {                                                                                                \
        TestResult result = {0};                                                                     \
                                                                                                     \
        RegionError error = REGION_ERROR_INIT;                                                       \
        Region *region = NULL;                                                                       \
                                                                                                     \
        region = api->region_alloc(sizeof(type) / 2, &error); UNWRAP;                                \
                                                                                                     \
        api->region_push_type(type, &region, TEST_REGION_PUSH_TYPE_CASE_2_VALUE, &error);            \
                                                                                                     \
        type *actual = (type *)(region->next->data + region->next->size - sizeof(type));             \
                                                                                                     \
        TEST_RESULT_WRITE_FMT(result, fmt, (type)TEST_REGION_PUSH_TYPE_CASE_2_VALUE, (type)*actual); \
                                                                                                     \
        api->region_free(&region);                                                                   \
                                                                                                     \
        return result;                                                                               \
                                                                                                     \
        TEST_FATAL(if (region) api->region_free(&region));                                           \
    }                                                                                                \

#define TEST_REGION_PUSH_TYPE_CASE_3(type, expected_error_code)         \
    TestResult test_region_push_## type ##_case_3(RegionAPI *api)       \
    {                                                                   \
        TestResult result = {0};                                        \
                                                                        \
        RegionError error = REGION_ERROR_INIT;                          \
                                                                        \
        api->region_push_type(type, NULL, 0, &error);                   \
                                                                        \
        TEST_RESULT_WRITE_INT(result, expected_error_code, error.code); \
                                                                        \
        return result;                                                  \
    }                                                                   \

#define TEST_REGION_PUSH_TYPE_CASE_4(type, expected_error_code)               \
    TestResult test_region_push_## type ##_case_4(RegionAPI *api)             \
    {                                                                         \
        TestResult result = {0};                                              \
                                                                              \
        RegionError error = REGION_ERROR_INIT;                                \
        Region *region = NULL;                                                \
                                                                              \
        region = api->region_alloc(sizeof(type) / 2, &error); UNWRAP;         \
                                                                              \
        api->test_set_available_memory(0);                                    \
                                                                              \
        api->region_push_type(type, &region, 0, &error);                      \
                                                                              \
        TEST_RESULT_WRITE_INT(result, expected_error_code, error.code);       \
                                                                              \
        api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT); \
                                                                              \
        api->region_free(&region);                                            \
                                                                              \
        return result;                                                        \
                                                                              \
        TEST_FATAL(if (region) api->region_free(&region));                    \
    }                                                                         \

TEST_REGION_PUSH_TYPE_CASE_1(int, "%d");
TEST_REGION_PUSH_TYPE_CASE_2(int, "%d");
TEST_REGION_PUSH_TYPE_CASE_3(int, REGION_ERROR_CODE_NO_ERROR);
TEST_REGION_PUSH_TYPE_CASE_4(int, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_INT_MALLOC_REGION);

TEST_REGION_PUSH_TYPE_CASE_1(float, "%f");
TEST_REGION_PUSH_TYPE_CASE_2(float, "%f");
TEST_REGION_PUSH_TYPE_CASE_3(float, REGION_ERROR_CODE_NO_ERROR);
TEST_REGION_PUSH_TYPE_CASE_4(float, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_FLOAT_MALLOC_REGION);

TEST_REGION_PUSH_TYPE_CASE_1(double, "%f");
TEST_REGION_PUSH_TYPE_CASE_2(double, "%f");
TEST_REGION_PUSH_TYPE_CASE_3(double, REGION_ERROR_CODE_NO_ERROR);
TEST_REGION_PUSH_TYPE_CASE_4(double, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_DOUBLE_MALLOC_REGION);

REGISTER_TEST(test_region_push_int_case_1, 1);
REGISTER_TEST(test_region_push_int_case_2, 2);
REGISTER_TEST(test_region_push_int_case_3, 3);
REGISTER_TEST(test_region_push_int_case_4, 4);

REGISTER_TEST(test_region_push_float_case_1, 1);
REGISTER_TEST(test_region_push_float_case_2, 2);
REGISTER_TEST(test_region_push_float_case_3, 3);
REGISTER_TEST(test_region_push_float_case_4, 4);

REGISTER_TEST(test_region_push_double_case_1, 1);
REGISTER_TEST(test_region_push_double_case_2, 2);
REGISTER_TEST(test_region_push_double_case_3, 3);
REGISTER_TEST(test_region_push_double_case_4, 4);

EXPORT_AT_TESTS_SECTION;