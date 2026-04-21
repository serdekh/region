#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "region-api.h"

#define FUNCS_SO_FILE_PATH "./.build/obj/funcs.so"
#define TEST_REGION_TEST_SO_FILE_PATH "../.build/sobj/region-test.so"

#define TEST_SECTION_TOKEN tests
#define TEST_SECTION "tests"

#define TEST_RESULT_MAX_STRING_SIZE 256

typedef struct {
    char actual[TEST_RESULT_MAX_STRING_SIZE];
    char expected[TEST_RESULT_MAX_STRING_SIZE];
    bool success;
    bool fatal;
} TestResult;

typedef struct {
    size_t case_number;
    const char *func_name;
    TestResult (*func)(RegionAPI *api);
} TestContext;

#define UNWRAP if (REGION_ERROR(error)) goto fatal

#define FMT_TO_STR(destination, fmt, value) sprintf((destination), (fmt), (value))

#define INT_TO_STR(to, n) FMT_TO_STR((to), "%d", (n))
#define PTR_TO_STR(to, p) FMT_TO_STR((to), "%p", (n))

#define TEST_RESULT_WRITE_FMT(result, fmt, e, a) \
    sprintf((result).expected, (fmt), (e));      \
    sprintf((result).actual, (fmt), (a));        \
    (result).success = (e) == (a);               \

#define TEST_RESULT_WRITE_INT(result, e, a) TEST_RESULT_WRITE_FMT((result), "%d", (e), (a))
#define TEST_RESULT_WRITE_FLOAT(result, e, a) TEST_RESULT_WRITE_FMT((result), "%f", (e), (a))
#define TEST_RESULT_WRITE_DOUBLE(result, e, a) TEST_RESULT_WRITE_FMT((result), "%f", ((double)e), ((double)a))
#define TEST_RESULT_WRITE_PTR(result, e, a) TEST_RESULT_WRITE_FMT((result), "%p", (e), (a))

#define TEST_LOG(stream, type, message, ...) fprintf((stream), "[Test]["type"]: "message, ##__VA_ARGS__)
#define TEST_LOG_LINE(stream, type, message, ...) fprintf((stream), "[Test]["type"]: "message"\n", ##__VA_ARGS__)
#define TEST_LOG_ERROR(message, ...) TEST_LOG(stderr, "Error", message, ##__VA_ARGS__)
#define TEST_LOG_ERROR_LINE(message, ...) TEST_LOG(stderr, "Error", message"\n", ##__VA_ARGS__)

#define TEST_LOG_PASSED_ALL_TESTS TEST_LOG_LINE(stdout, "Log", "All tests have passed!")

#define TEST_LOG_PASSED_TEST(test_context) TEST_LOG_LINE(stdout, "Check", "[function: \"%s\"][case: %zu]: Passing", (test_context)->func_name, (test_context)->case_number)

#define TEST_LOG_FAILED_TEST(test_context, result) TEST_LOG_LINE(stderr, "Check", "[function: \"%s\"][case: %zu]: Failed!\n\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n",\
        (test_context)->func_name, (test_context)->case_number, result.expected, result.actual)


#define TEST_LOG_ERROR_FAILED_TEST_FATAL_ERROR                       \
    TEST_LOG(stderr, "Error", "Could not perfom a test. Stop.\n\t"); \
    REGION_LOG_ERROR(error)

#define TEST_FATAL(cleanup_code)             \
fatal:                                       \
    TEST_LOG_ERROR_FAILED_TEST_FATAL_ERROR;  \
    cleanup_code;                            \
    result.fatal = true;                     \
    return result;                           \

#define GET_TESTS_START_STR   "get_tests_start"
#define GET_TESTS_END_STR   "get_tests_end"

#define EXPOSE(test_section_name)                              \
    extern const TestContext __start_##test_section_name[];    \
    extern const TestContext __stop_##test_section_name[];     \
    __attribute__((visibility("default")))                     \
    const TestContext *get_tests_start(void) {                 \
        return __start_##test_section_name;                    \
    }                                                          \
    __attribute__((visibility("default")))                     \
    const TestContext *get_tests_end(void) {                   \
        return __stop_##test_section_name;                     \
    }

#define REGISTER_TEST(f, num)                                                       \
    static const TestContext test_##f                                               \
    __attribute__((section(TEST_SECTION), used, aligned(sizeof(void*)))) = {        \
        .case_number = num,                                                         \
        .func_name = #f,                                                            \
        .func = f,                                                                  \
    };                        

#define EXPORT_AT_TESTS_SECTION EXPOSE(tests)