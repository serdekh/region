#pragma once

#include "../include/shared.h"
#include "../../../src/region.h"

#define REGION_SHARED_OBJECT_FILE_PATH "../.build/obj/region.so"

typedef Region*(*FuncPtr_region_alloc)(size_t, RegionError*, RegionLocation);

extern void *_RegionHandle;

void try_get_region_handle();
void *try_get_symbol(const char *symbol_name);