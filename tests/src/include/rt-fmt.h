#pragma once

#include <stdio.h>

#include "rt-TestResult.h"

#define RT_FMT_TO_STR(destination, fmt, value) sprintf((destination), (fmt), (value))
#define RT_INT_TO_STR(to, n)                   RT_FMT_TO_STR((to), "%d", (n))
#define RT_PTR_TO_STR(to, p)                   RT_FMT_TO_STR((to), "%p", (n))

#define RT_TEST_RESULT_WRITE_FMT(result, fmt, e, a) \
    sprintf((result).expected, (fmt), (e));         \
    sprintf((result).actual, (fmt), (a));           \
    (result).success = (e) == (a);                  \

#define RT_TEST_RESULT_WRITE_INT(result, e, a)    RT_TEST_RESULT_WRITE_FMT((result), "%d",    (int)(e),    (int)(a))
#define RT_TEST_RESULT_WRITE_FLOAT(result, e, a)  RT_TEST_RESULT_WRITE_FMT((result), "%f",  (float)(e),  (float)(a))
#define RT_TEST_RESULT_WRITE_DOUBLE(result, e, a) RT_TEST_RESULT_WRITE_FMT((result), "%f", (double)(e), (double)(a))
#define RT_TEST_RESULT_WRITE_PTR(result, e, a)    RT_TEST_RESULT_WRITE_FMT((result), "%p",         (e),         (a))
