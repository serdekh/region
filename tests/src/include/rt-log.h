#pragma once

#include <stdio.h>

#include "rt-RegionAPI.h"
#include "rt-TestResult.h"
#include "rt-TestContext.h"

#define RT_LOG(stream, type, message, ...)      fprintf((stream), "[Test]["type"]: "message,     ##__VA_ARGS__)
#define RT_LOG_LINE(stream, type, message, ...) fprintf((stream), "[Test]["type"]: "message"\n", ##__VA_ARGS__)

#define RT_LOG_ERROR(message, ...)      RT_LOG(stderr, "Error", message,     ##__VA_ARGS__)
#define RT_LOG_ERROR_LINE(message, ...) RT_LOG(stderr, "Error", message"\n", ##__VA_ARGS__)

#define RT_LOG_PASSED_TEST(test_context) RT_LOG_LINE(stdout, "Check", "[function: \"%s\"][case: %zu]: Passing", \
	(test_context)->func_name, (test_context)->case_number)

#define RT_LOG_FAILED_TEST(test_context, result) RT_LOG_LINE(stderr, "Check", "[function: \"%s\"][case: %zu]: Failed!\n\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n",\
    (test_context)->func_name, (test_context)->case_number, result.expected, result.actual)

#define RT_LOG_ERROR_FAILED_TEST_FATAL_ERROR                       \
    RT_LOG(stderr, "Error", "Could not perfom a test. Stop.\n\t"); \
    REGION_LOG_ERROR(error)
    
#define RT_LOG_PASSED_ALL_TESTS RT_LOG_LINE(stdout, "Log", "All tests have passed!")
