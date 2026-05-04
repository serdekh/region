#pragma once

#include "rt-RegionAPI.h"
#include "rt-TestResult.h"

typedef struct {
    size_t case_number;
    const char *func_name;
    TestResult (*func)(RegionAPI *api);
} TestContext;
