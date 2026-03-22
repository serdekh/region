#pragma once

#include <stdio.h>
#include <stdbool.h>

#define FUNCS_SO_FILE_PATH "./.build/obj/funcs.so"

#define TEST_SECTION_TOKEN tests
#define TEST_SECTION "tests"

typedef struct {
    size_t case_number;
    const char *func_name;
    int (*func)(void);
    bool passed;
} TestContext;

#define EXPOSE(test_section_name)                           \
    extern const TestContext __start_##test_section_name[]; \
    extern const TestContext __stop_##test_section_name[];  \
    const TestContext *get_tests_start(void) {              \
        return __start_##test_section_name;                 \
    }                                                       \
    const TestContext *get_tests_end(void) {                \
        return __stop_##test_section_name;                  \
    }                                                                                                 

#define REGISTER_TEST(f, num)                               \
    static const TestContext test_##func                    \
    __attribute__((section(TEST_SECTION), used)) = {        \
        .case_number = num,                                 \
        .func_name = #f,                                    \
        .func = f,                                          \
        .passed = false                                     \
    };                        
