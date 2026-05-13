#pragma once

#include "rt-RegionAPI.h"

#include "rt-TestResult.h"
#include "rt-TestContext.h"

#include "rt-so.h"
#include "rt-log.h"
#include "rt-fmt.h"
#include "rt-file-paths.h"
#include "rt-test-module.h"

#define RT_TARGET_UNWRAP if ((error).type != REGION_ERROR_TYPE_NONE) goto fatal

#define RT_TARGET_FATAL_ERROR(cleanup_code)      \
    fatal:                                       \
        RT_LOG_ERROR_FAILED_TEST_FATAL_ERROR;    \
        cleanup_code;                            \
        result.fatal = true;                     \
        return result;                           

#define RT_REGION_CORE_FIELDS \
    size_t size;              \
    size_t capacity;          \
    char *data;               \
    struct __Region *next;    \

typedef struct __Region      { RT_REGION_CORE_FIELDS } Region;
typedef struct __StackRegion { RT_REGION_CORE_FIELDS } StackRegion;