#include "../include/shared.h"

#define TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_0 232
#define TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_1 563399
#define TEST_STACK_REGION_POP_TYPE_CASE_4_VALUE 33219

#define stack_region_push_type(type, stack, size, error) stack_region_push_##type(stack, size, error)
#define stack_region_pop_type(type, stack, error) stack_region_pop_##type(stack, error)

#define TEST_STACK_REGION_POP_TYPE_CASE_1(type)    \
    TestResult test_stack_region_pop_## type ##_case_1(RegionAPI *api)  \
    {                                                                   \
        TestResult result = {0};                                        \
                                                                        \
        RegionError error = REGION_ERROR_INIT;                          \
                                                                        \
        TEST_RESULT_WRITE_PTR(result, NULL,                             \
            api->stack_region_pop_type(type, NULL, &error));            \
                                                                        \
        return result;                                                  \
    }                                                                   \

#define TEST_STACK_REGION_POP_TYPE_CASE_2(type, expected_error_code)                        \
    TestResult test_stack_region_pop_## type ##_case_2(RegionAPI *api)                      \
    {                                                                                       \
        TestResult result = {0};                                                            \
                                                                                            \
        RegionError error = REGION_ERROR_INIT;                                              \
                                                                                            \
        StackRegion *stack = NULL;                                                          \
                                                                                            \
        stack = api->stack_region_alloc(sizeof(type) * 2 + sizeof(size_t), &error); UNWRAP; \
                                                                                            \
        api->stack_region_push(&stack, sizeof(type) * 2, &error); UNWRAP;                   \
                                                                                            \
        api->stack_region_pop_type(type, stack, &error);                                    \
                                                                                            \
        api->stack_region_free(&stack);                                                     \
                                                                                            \
        TEST_RESULT_WRITE_INT(result, expected_error_code, error.code);                     \
                                                                                            \
        return result;                                                                      \
                                                                                            \
        TEST_FATAL(if (stack) api->stack_region_free(&stack));                              \
    }                                                                                       \

#define TEST_STACK_REGION_POP_TYPE_CASE_3(type, fmt)                                                                  \
    TestResult test_stack_region_pop_## type ##_case_3(RegionAPI *api)                                                \
    {                                                                                                                 \
        TestResult result = {0};                                                                                      \
                                                                                                                      \
        RegionError error = REGION_ERROR_INIT;                                                                        \
                                                                                                                      \
        StackRegion *stack = NULL;                                                                                    \
                                                                                                                      \
        stack = api->stack_region_alloc(sizeof(type) * 2 + sizeof(size_t) * 2, &error); UNWRAP;                       \
                                                                                                                      \
        api->stack_region_push_type(type, &stack, TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_1, &error); UNWRAP;\
        api->stack_region_push_type(type, &stack, TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_0, &error); UNWRAP;\
                                                                                                                      \
        type *at_index_0 = api->stack_region_pop_type(type, stack, &error);                                           \
        type *at_index_1 = api->stack_region_pop_type(type, stack, &error);                                           \
                                                                                                                      \
        sprintf(result.expected, "At index '1': "fmt", At index '0': "fmt,                                            \
            (type)TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_1,                                                 \
            (type)TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_0);                                                \
                                                                                                                      \
        sprintf(result.actual, "At index '1': "fmt", At index '0': "fmt,                                              \
            (type)*at_index_1,                                                                                        \
            (type)*at_index_0);                                                                                       \
                                                                                                                      \
        result.success = (TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_1 == *at_index_1) &&                       \
                        (TEST_STACK_REGION_POP_TYPE_CASE_3_VALUE_AT_INDEX_0 == *at_index_0);                          \
                                                                                                                      \
        api->stack_region_free(&stack);                                                                               \
                                                                                                                      \
        return result;                                                                                                \
                                                                                                                      \
        TEST_FATAL(if (stack) api->stack_region_free(&stack));                                                        \
    }                                                                                                                 \

#define TEST_STACK_REGION_POP_TYPE_CASE_4(type, fmt)                                                          \
    TestResult test_stack_region_pop_## type ##_case_4(RegionAPI *api)                                        \
    {                                                                                                         \
        TestResult result = {0};                                                                              \
                                                                                                              \
        RegionError error = REGION_ERROR_INIT;                                                                \
                                                                                                              \
        StackRegion *stack = NULL;                                                                            \
                                                                                                              \
        stack = api->stack_region_alloc(sizeof(type) / 2, &error); UNWRAP;                                    \
                                                                                                              \
        api->stack_region_push_type(type, &stack, TEST_STACK_REGION_POP_TYPE_CASE_4_VALUE, &error); UNWRAP;   \
                                                                                                              \
        type *at_index_0 = api->stack_region_pop_type(type, stack, &error);                                   \
                                                                                                              \
        TEST_RESULT_WRITE_FMT(result, fmt, (type)TEST_STACK_REGION_POP_TYPE_CASE_4_VALUE, (type)*at_index_0); \
                                                                                                              \
        api->stack_region_free(&stack);                                                                       \
                                                                                                              \
        return result;                                                                                        \
                                                                                                              \
        TEST_FATAL(if (stack) api->stack_region_free(&stack));                                                \
    }                                                                                                         \

TEST_STACK_REGION_POP_TYPE_CASE_1(int);
TEST_STACK_REGION_POP_TYPE_CASE_2(int, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_INT_INVALID_FRAME);
TEST_STACK_REGION_POP_TYPE_CASE_3(int, "%d");
TEST_STACK_REGION_POP_TYPE_CASE_4(int, "%d");

TEST_STACK_REGION_POP_TYPE_CASE_1(float);
TEST_STACK_REGION_POP_TYPE_CASE_2(float, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_FLOAT_INVALID_FRAME);
TEST_STACK_REGION_POP_TYPE_CASE_3(float, "%f");
TEST_STACK_REGION_POP_TYPE_CASE_4(float, "%f");

TEST_STACK_REGION_POP_TYPE_CASE_1(double);
TEST_STACK_REGION_POP_TYPE_CASE_2(double, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_DOUBLE_INVALID_FRAME);
TEST_STACK_REGION_POP_TYPE_CASE_3(double, "%f");
TEST_STACK_REGION_POP_TYPE_CASE_4(double, "%f");

REGISTER_TEST(test_stack_region_pop_int_case_1, 1);
REGISTER_TEST(test_stack_region_pop_int_case_2, 2);
REGISTER_TEST(test_stack_region_pop_int_case_3, 3);
REGISTER_TEST(test_stack_region_pop_int_case_4, 4);

REGISTER_TEST(test_stack_region_pop_float_case_1, 1);
REGISTER_TEST(test_stack_region_pop_float_case_2, 2);
REGISTER_TEST(test_stack_region_pop_float_case_3, 3);
REGISTER_TEST(test_stack_region_pop_float_case_4, 4);

REGISTER_TEST(test_stack_region_pop_double_case_1, 1);
REGISTER_TEST(test_stack_region_pop_double_case_2, 2);
REGISTER_TEST(test_stack_region_pop_double_case_3, 3);
REGISTER_TEST(test_stack_region_pop_double_case_4, 4);

EXPORT_AT_TESTS_SECTION;