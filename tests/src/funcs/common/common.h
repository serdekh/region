#pragma once

#include "../../include/shared.h"
#include "../../../../src/region.h"

#define REGION_SHARED_OBJECT_FILE_PATH "../.build/obj/region-test.so"

#define SYMBOL_FN_REGION_ALLOC "region_alloc"
#define SYMBOL_FN_REGION_PUSH "region_push"
#define SYMBOL_FN_REGION_SHRINK_CAPACITY "region_shrink_capacity"
#define SYMBOL_FN_REGION_GET_LAST_NODE "region_get_last_node"
#define SYMBOL_FN_REGION_COLLECT "region_collect"
#define SYMBOL_FN_REGION_CLONE "region_clone"
#define SYMBOL_FN_REGION_MERGE "region_merge"
#define SYMBOL_FN_REGION_FREE "region_free"

#define SYMBOL_FN_STACK_REGION_ALLOC "stack_region_alloc"
#define SYMBOL_FN_STACK_REGION_PUSH "stack_region_push"
#define SYMBOL_FN_STACK_REGION_PEEK "stack_region_peek"
#define SYMBOL_FN_STACK_REGION_PEEK_AT "stack_region_peek_at"
#define SYMBOL_FN_STACK_REGION_POP "stack_region_pop"
#define SYMBOL_FN_STACK_REGION_FREE "stack_region_free"

#define SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY "__test_set_available_memory"
#define SYMBOL_FN_TEST_GET_AVAILABLE_MEMORY "__test_get_available_memory"

typedef Region *(*FuncPtr_region_alloc)(size_t, RegionError*);
typedef Region *(*FuncPtr_region_get_last_node)(Region *region, RegionError *error);
typedef Region *(*FuncPtr_region_clone)(Region *region, RegionError *error);
typedef Region *(*FuncPtr_region_merge)(Region *region, RegionMergeOption option, RegionError *error);
typedef Region **(*FuncPtr_region_collect)(Region *region, size_t *collected_size, RegionError *error);
typedef void *(*FuncPtr_region_push)(Region *region, size_t size, RegionError *error);
typedef void (*FuncPtr_region_shrink_capacity)(Region *region, RegionShrinkCapacityOption option, RegionError *error);
typedef void (*FuncPtr_region_free)(Region **region);

typedef StackRegion *(*FuncPtr_stack_region_alloc)(size_t capacity, RegionError *error);
typedef void *(*FuncPtr_stack_region_push)(StackRegion *stack, size_t size, RegionError *error);
typedef void *(*FuncPtr_stack_region_peek)(StackRegion *stack, RegionError *error);
typedef void *(*FuncPtr_stack_region_peek_at)(StackRegion *stack, size_t index, RegionError *error);
typedef void *(*FuncPtr_stack_region_pop)(StackRegion *stack, RegionError *error);
typedef void (*FuncPtr_stack_region_free)(StackRegion **stack);

typedef void (*FuncPtr_test_set_available_memory)(size_t value);
typedef size_t (*FuncPtr_test_get_available_memory)();

extern void *_RegionHandle;
extern FuncPtr_test_set_available_memory __test_set_available_memory;
extern FuncPtr_test_get_available_memory __test_get_available_memory;

void try_get_region_handle();
void *try_get_symbol(const char *symbol_name);
void set_available_memory(size_t value);
size_t get_available_memory();

#define UNWRAP if(REGION_ERROR(error)) goto fatal