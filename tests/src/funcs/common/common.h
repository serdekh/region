#pragma once

#include "../../include/shared.h"
#include "../../../../src/region.h"

#define REGION_SHARED_OBJECT_FILE_PATH "../.build/obj/region-test.so"

#define SYMBOL_FN_REGION_ALLOC "__region_alloc"
#define SYMBOL_FN_REGION_PUSH "__region_push"
#define SYMBOL_FN_REGION_SHRINK_CAPACITY "__region_shrink_capacity"
#define SYMBOL_FN_REGION_COLLECT "__region_collect"
#define SYMBOL_FN_REGION_CLONE "__region_clone"
#define SYMBOL_FN_REGION_MERGE "__region_merge"

#define SYMBOL_FN_REGION_FREE "region_free"
#define SYMBOL_FN_REGION_GET_LAST_NODE "region_get_last_node"
#define SYMBOL_FN_STACK_REGION_FREE "stack_region_free"

#define SYMBOL_FN_STACK_REGION_ALLOC "__stack_region_alloc"
#define SYMBOL_FN_STACK_REGION_PUSH "__stack_region_push"
#define SYMBOL_FN_STACK_REGION_PEEK "__stack_region_peek"
#define SYMBOL_FN_STACK_REGION_POP "__stack_region_pop"

#define SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY "__test_set_available_memory"
#define SYMBOL_FN_TEST_GET_AVAILABLE_MEMORY "__test_get_available_memory"

typedef Region*(*FuncPtr_region_alloc)(size_t, RegionError*, RegionLocation);
typedef void *(*FuncPtr_region_push)(Region *region, size_t size, RegionError *error, RegionLocation location);
typedef void (*FuncPtr_region_shrink_capacity)(Region *region, RegionShrinkCapacityOption option, RegionError *error, RegionLocation location);
typedef Region **(*FuncPtr_region_collect)(Region *region, size_t *collected_size, RegionError *error, RegionLocation location);
typedef Region *(*FuncPtr_region_clone)(Region *region, RegionError *error, RegionLocation location);
typedef Region *(*FuncPtr_region_merge)(Region *region, RegionMergeOption option, RegionError *error, RegionLocation location);

typedef void (*FuncPtr_region_free)(Region **region);
typedef Region *(*FuncPtr_region_get_last_node)(Region *region, RegionError *error, RegionLocation location);
typedef void (*FuncPtr_stack_region_free)(StackRegion **stack);

typedef StackRegion *(*FuncPtr_stack_region_alloc)(size_t capacity, RegionError *error, RegionLocation location);
typedef void *(*FuncPtr_stack_region_push)(StackRegion *stack, size_t size, RegionError *error, RegionLocation location);
typedef void *(*FuncPtr_stack_region_peek)(StackRegion *stack, RegionError *error, RegionLocation location);
typedef void *(*FuncPtr_stack_region_pop)(StackRegion *stack, RegionError *error, RegionLocation location);

typedef void (*FuncPtr_test_set_available_memory)(size_t value);
typedef size_t (*FuncPtr_test_get_available_memory)();

extern void *_RegionHandle;
extern FuncPtr_test_set_available_memory __test_set_available_memory;
extern FuncPtr_test_get_available_memory __test_get_available_memory;

void try_get_region_handle();
void *try_get_symbol(const char *symbol_name);
void set_available_memory(size_t value);
size_t get_available_memory();