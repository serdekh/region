#pragma once

#include "../../include/shared.h"
#include "../../../../src/region.h"

#define REGION_SHARED_OBJECT_FILE_PATH "../.build/obj/region.so"

#define SYMBOL_FN_REGION_ALLOC "__region_alloc"
#define SYMBOL_FN_REGION_PUSH "__region_push"
#define SYMBOL_FN_REGION_SHRINK_CAPACITY "__region_shrink_capacity"
#define SYMBOL_FN_REGION_FREE "region_free"


typedef Region*(*FuncPtr_region_alloc)(size_t, RegionError*, RegionLocation);
typedef void *(*FuncPtr_region_push)(Region *region, size_t size, RegionError *error, RegionLocation location);
typedef void (*FuncPtr_region_shrink_capacity)(Region *region, RegionError *error, RegionLocation location);
typedef void (*FuncPtr_region_free)(Region **region);

extern void *_RegionHandle;

void try_get_region_handle();
void *try_get_symbol(const char *symbol_name);