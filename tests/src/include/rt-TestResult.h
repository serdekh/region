#pragma once

#define RT_TEST_RESULT_MAX_STRING_SIZE 256

typedef struct {
    char actual[RT_TEST_RESULT_MAX_STRING_SIZE];
    char expected[RT_TEST_RESULT_MAX_STRING_SIZE];
    bool success;
    bool fatal;
} TestResult;
