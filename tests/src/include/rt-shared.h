#pragma once

#include "rt-RegionAPI.h"

#include "rt-TestResult.h"
#include "rt-TestContext.h"

#include "rt-so.h"
#include "rt-log.h"
#include "rt-fmt.h"
#include "rt-file-paths.h"
#include "rt-test-module.h"

#define RT_TARGET_UNWRAP if (REGION_ERROR(error)) goto fatal

#define RT_TARGET_FATAL_ERROR(cleanup_code)      \
    fatal:                                       \
        RT_LOG_ERROR_FAILED_TEST_FATAL_ERROR;    \
        cleanup_code;                            \
        result.fatal = true;                     \
        return result;                           
        