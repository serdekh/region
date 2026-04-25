#pragma once

#include "rt-TestContext.h"

#define RT_GET_TEST_MODULE_START      _rt_get_test_module_start
#define RT_GET_TEST_MODULE_START_STR "_rt_get_test_module_start"

#define RT_GET_TEST_MODULE_END        _rt_get_test_module_end
#define RT_GET_TEST_MODULE_END_STR   "_rt_get_test_module_end"

#define RT_TEST_MODULE_EXPORT                                           \
    extern const TestContext __start_rt_test_module[];    \
    extern const TestContext __stop_rt_test_module[];      \
                                                                        \
    __attribute__((visibility("default")))                              \
    const TestContext *RT_GET_TEST_MODULE_START(void) { \
        return __start_rt_test_module;                    \
    }                                                                   \
                                                                        \
    __attribute__((visibility("default")))                              \
    const TestContext *RT_GET_TEST_MODULE_END(void) {  \
        return __stop_rt_test_module;                      \
    }

#define RT_TEST_MODULE_REGISTER(f, num)                                             \
    static const TestContext __rt_target_##f                                               \
                                                                                    \
    __attribute__((section("rt_test_module"), used, aligned(sizeof(void*)))) = {        \
        .case_number = num,                                                         \
        .func_name = #f,                                                            \
        .func = f,                                                                  \
    };                        
