#include "../include/rt-shared.h"

#define TEST_STACK_REGION_PUSH_TYPE_CASE2_STATIC_DATA "TEST_STACK_REGION_PUSH_TYPE_CASE2_STATIC_DATA"
#define TEST_STACK_REGION_PUSH_TYPE_CASE2_STATIC_DATA_LENGTH sizeof(TEST_STACK_REGION_PUSH_TYPE_CASE2_STATIC_DATA)
#define TEST_STACK_REGION_PUSH_TYPE_CASE3_VALUE 123
#define TEST_STACK_REGION_PUSH_TYPE_CASE4_VALUE 222

#define stack_region_push_type(type, stack, value, error) stack_region_push_##type(stack, value, error)

#define TEST_STACK_REGION_PUSH_TYPE_CASE_1(type)                        \
    TestResult test_stack_region_push_## type ##_case_1(RegionAPI *api) \
    {                                                                   \
        TestResult result = {0};                                        \
                                                                        \
        RegionError error = REGION_ERROR_INIT;                          \
                                                                        \
        RT_TEST_RESULT_WRITE_PTR(result, NULL,                          \
            api->stack_region_push_##type(NULL, 0, &error));            \
                                                                        \
        return result;                                                  \
    }                                                                   \

#define TEST_STACK_REGION_PUSH_TYPE_CASE_2(type, expected_error_code)          \
    TestResult test_stack_region_push_## type ##_case_2(RegionAPI *api)        \
    {                                                                          \
        TestResult result = {0};                                               \
                                                                               \
        RegionError error = REGION_ERROR_INIT;                                 \
                                                                               \
        StackRegion stack = {                                                  \
            .next     = NULL,                                                  \
            .data     = TEST_STACK_REGION_PUSH_TYPE_CASE2_STATIC_DATA,         \
            .capacity = TEST_STACK_REGION_PUSH_TYPE_CASE2_STATIC_DATA_LENGTH,  \
            .size     = TEST_STACK_REGION_PUSH_TYPE_CASE2_STATIC_DATA_LENGTH   \
        };                                                                     \
                                                                               \
        api->test_set_available_memory(0);                                     \
            StackRegion *pstack = &stack;                                      \
            api->stack_region_push_type(type, &pstack, 0, &error);             \
        api->test_set_available_memory(REGION_TEST_AVAILABLE_MEMORY_DEFAULT);  \
                                                                               \
        RT_TEST_RESULT_WRITE_INT(result, expected_error_code, error.code);     \
                                                                               \
        return result;                                                         \
    }                                                                          \

#define TEST_STACK_REGION_PUSH_TYPE_CASE_3(type, fmt)                                                             \
    TestResult test_stack_region_push_## type ##_case_3(RegionAPI *api)                                           \
    {                                                                                                             \
        TestResult result = {0};                                                                                  \
                                                                                                                  \
        RegionError error = REGION_ERROR_INIT;                                                                    \
                                                                                                                  \
        StackRegion *stack = NULL;                                                                                \
                                                                                                                  \
        stack = api->stack_region_alloc(sizeof(type) + sizeof(size_t), &error); RT_TARGET_UNWRAP;                 \
                                                                                                                  \
        type *pushed = api->stack_region_push_type(type, &stack, TEST_STACK_REGION_PUSH_TYPE_CASE3_VALUE, &error);\
                                                                                                                  \
        type *peeked = (type *)(stack->data);                                                                     \
                                                                                                                  \
        if (pushed != peeked) {                                                                                   \
            RT_TEST_RESULT_WRITE_PTR(result, pushed, peeked);                                                     \
            api->stack_region_free(&stack);                                                                       \
            return result;                                                                                        \
        }                                                                                                         \
                                                                                                                  \
        RT_TEST_RESULT_WRITE_FMT(result, fmt, *pushed, *peeked);                                                  \
                                                                                                                  \
        api->stack_region_free(&stack);                                                                           \
                                                                                                                  \
        return result;                                                                                            \
                                                                                                                  \
        RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));                                         \
    }                                                                                                             \

#define TEST_STACK_REGION_PUSH_TYPE_CASE_4(type, fmt)                                                             \
    TestResult test_stack_region_push_## type ##_case_4(RegionAPI *api)                                           \
    {                                                                                                             \
        TestResult result = {0};                                                                                  \
                                                                                                                  \
        RegionError error = REGION_ERROR_INIT;                                                                    \
                                                                                                                  \
        StackRegion *stack = NULL;                                                                                \
                                                                                                                  \
        stack = api->stack_region_alloc(sizeof(type) / 2, &error); RT_TARGET_UNWRAP;                              \
                                                                                                                  \
        type *pushed = api->stack_region_push_type(type, &stack, TEST_STACK_REGION_PUSH_TYPE_CASE4_VALUE, &error);\
                                                                                                                  \
        type *peeked = (type *)(stack->next->data);                                                               \
                                                                                                                  \
        if (pushed != peeked) {                                                                                   \
            RT_TEST_RESULT_WRITE_PTR(result, pushed, peeked);                                                     \
            api->stack_region_free(&stack);                                                                       \
            return result;                                                                                        \
        }                                                                                                         \
                                                                                                                  \
        RT_TEST_RESULT_WRITE_FMT(result, fmt, *pushed, *peeked);                                                  \
                                                                                                                  \
        api->stack_region_free(&stack);                                                                           \
                                                                                                                  \
        return result;                                                                                            \
                                                                                                                  \
        RT_TARGET_FATAL_ERROR(if (stack) api->stack_region_free(&stack));                                         \
    }                                                                                                             \

TEST_STACK_REGION_PUSH_TYPE_CASE_1(int);
TEST_STACK_REGION_PUSH_TYPE_CASE_2(int, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_INT_MALLOC_REGION);
TEST_STACK_REGION_PUSH_TYPE_CASE_3(int, "%d");
TEST_STACK_REGION_PUSH_TYPE_CASE_4(int, "%d");

TEST_STACK_REGION_PUSH_TYPE_CASE_1(float);
TEST_STACK_REGION_PUSH_TYPE_CASE_2(float, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_FLOAT_MALLOC_REGION);
TEST_STACK_REGION_PUSH_TYPE_CASE_3(float, "%f");
TEST_STACK_REGION_PUSH_TYPE_CASE_4(float, "%f");

TEST_STACK_REGION_PUSH_TYPE_CASE_1(double);
TEST_STACK_REGION_PUSH_TYPE_CASE_2(double, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_DOUBLE_MALLOC_REGION);
TEST_STACK_REGION_PUSH_TYPE_CASE_3(double, "%f");
TEST_STACK_REGION_PUSH_TYPE_CASE_4(double, "%f");

TEST_STACK_REGION_PUSH_TYPE_CASE_1(char);
TEST_STACK_REGION_PUSH_TYPE_CASE_2(char, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_CHAR_MALLOC_REGION);
TEST_STACK_REGION_PUSH_TYPE_CASE_3(char, "%c");
TEST_STACK_REGION_PUSH_TYPE_CASE_4(char, "%c");

RT_TEST_MODULE_REGISTER(test_stack_region_push_int_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_push_int_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_push_int_case_3, 3);
RT_TEST_MODULE_REGISTER(test_stack_region_push_int_case_4, 4);

RT_TEST_MODULE_REGISTER(test_stack_region_push_float_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_push_float_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_push_float_case_3, 3);
RT_TEST_MODULE_REGISTER(test_stack_region_push_float_case_4, 4);

RT_TEST_MODULE_REGISTER(test_stack_region_push_double_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_push_double_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_push_double_case_3, 3);
RT_TEST_MODULE_REGISTER(test_stack_region_push_double_case_4, 4);

RT_TEST_MODULE_REGISTER(test_stack_region_push_char_case_1, 1);
RT_TEST_MODULE_REGISTER(test_stack_region_push_char_case_2, 2);
RT_TEST_MODULE_REGISTER(test_stack_region_push_char_case_3, 3);
RT_TEST_MODULE_REGISTER(test_stack_region_push_char_case_4, 4);

RT_TEST_MODULE_EXPORT;