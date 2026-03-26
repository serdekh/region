#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <dlfcn.h>

#define FUNCS_SO_FILE_PATH "./.build/obj/funcs.so"

#define TEST_SECTION_TOKEN tests
#define TEST_SECTION "tests"

#define TEST_RESULT_MAX_STRING_SIZE 256

typedef struct {
    char actual[TEST_RESULT_MAX_STRING_SIZE];
    char expected[TEST_RESULT_MAX_STRING_SIZE];
    bool success;
} TestResult;

typedef struct {
    size_t case_number;
    const char *func_name;
    TestResult (*func)(void);
} TestContext;

#define INT_TO_STR(to, n) sprintf((to), "%d", n)

#define TEST_RESULT_WRITE_INT(result, e, a)                    \
    INT_TO_STR((result).expected, (e));                        \
    INT_TO_STR((result).actual, (a));                          \
    (result).success = (e) == (a);                             \

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
