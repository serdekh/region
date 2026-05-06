#pragma once
#include "../../../src/region.h"

#define SYMBOL_FN_REGION_ALLOC               "region_alloc"
#define SYMBOL_FN_REGION_FREE                "region_free"

#define SYMBOL_FN_REGION_GET_CAPACITY        "region_get_capacity"
#define SYMBOL_FN_REGION_GET_SIZE            "region_get_size"

#define SYMBOL_FN_REGION_PUSH                "region_push"
#define SYMBOL_FN_REGION_PUSH_INT            "region_push_int"
#define SYMBOL_FN_REGION_PUSH_FLOAT          "region_push_float"
#define SYMBOL_FN_REGION_PUSH_DOUBLE         "region_push_double"
#define SYMBOL_FN_REGION_PUSH_CHAR           "region_push_char"

#define SYMBOL_FN_REGION_SHRINK_CAPACITY     "region_shrink_capacity"
#define SYMBOL_FN_REGION_GET_LAST_NODE       "region_get_last_node"
#define SYMBOL_FN_REGION_COLLECT             "region_collect"
#define SYMBOL_FN_REGION_CLONE               "region_clone"
#define SYMBOL_FN_REGION_MERGE               "region_merge"

#define SYMBOL_FN_STACK_REGION_ALLOC         "stack_region_alloc"
#define SYMBOL_FN_STACK_REGION_FREE          "stack_region_free"

#define SYMBOL_FN_STACK_REGION_GET_CAPACITY  "stack_region_get_capacity"
#define SYMBOL_FN_STACK_REGION_GET_SIZE      "stack_region_get_size"
#define SYMBOL_FN_STACK_REGION_GET_COUNT     "stack_region_get_count"
#define SYMBOL_FN_STACK_REGION_GET_COUNT_REF "stack_region_get_count_ref"

#define SYMBOL_FN_STACK_REGION_PUSH          "stack_region_push"
#define SYMBOL_FN_STACK_REGION_PUSH_INT      "stack_region_push_int"
#define SYMBOL_FN_STACK_REGION_PUSH_FLOAT    "stack_region_push_float"
#define SYMBOL_FN_STACK_REGION_PUSH_DOUBLE   "stack_region_push_double"
#define SYMBOL_FN_STACK_REGION_PUSH_CHAR     "stack_region_push_char"

#define SYMBOL_FN_STACK_REGION_POP           "stack_region_pop"
#define SYMBOL_FN_STACK_REGION_POP_INT       "stack_region_pop_int"
#define SYMBOL_FN_STACK_REGION_POP_FLOAT     "stack_region_pop_float"
#define SYMBOL_FN_STACK_REGION_POP_DOUBLE    "stack_region_pop_double"
#define SYMBOL_FN_STACK_REGION_POP_CHAR      "stack_region_pop_char"

#define SYMBOL_FN_STACK_REGION_PEEK          "stack_region_peek"
#define SYMBOL_FN_STACK_REGION_PEEK_AT       "stack_region_peek_at"

#define SYMBOL_FN_STACK_REGION_SWAP          "stack_region_swap"

#define SYMBOL_FN_REGION_ERROR_PRINT_TO      "region_error_print_to"
#define SYMBOL_FN_REGION_ERROR_PRINT         "region_error_print"

#define SYMBOL_FN_TEST_SET_AVAILABLE_MEMORY  "__test_set_available_memory"
#define SYMBOL_FN_TEST_SET_DEFAULT_MEMORY    "__test_set_default_memory"

#define SYMBOL_FN_TEST_GET_AVAILABLE_MEMORY  "__test_get_available_memory"     

typedef Region *(*FuncPtr_region_alloc)(size_t, RegionError*);

typedef size_t (*FuncPtr_region_get_capacity)(Region *region);
typedef size_t (*FuncPtr_region_get_size)(Region *region);

typedef Region *(*FuncPtr_region_get_last_node)(Region *region, RegionGetLastNodeOption option, RegionError *error);
typedef Region *(*FuncPtr_region_clone)(Region *region, RegionError *error);
typedef Region *(*FuncPtr_region_merge)(Region *region, RegionMergeOption option, RegionError *error);
typedef Region **(*FuncPtr_region_collect)(Region *region, size_t *collected_size, RegionError *error);

typedef void *(*FuncPtr_region_push)(Region **region, size_t size, RegionError *error);
typedef int *(*FuncPtr_region_push_int)(Region **region, int value, RegionError *error);
typedef float *(*FuncPtr_region_push_float)(Region **region, float value, RegionError *error);
typedef double *(*FuncPtr_region_push_double)(Region **region, double value, RegionError *error);
typedef char *(*FuncPtr_region_push_char)(Region **region, char value, RegionError *error);

typedef void (*FuncPtr_region_shrink_capacity)(Region *region, RegionShrinkCapacityOption option, RegionError *error);
typedef void (*FuncPtr_region_free)(Region **region);

typedef StackRegion *(*FuncPtr_stack_region_alloc)(size_t capacity, RegionError *error);

typedef size_t (*FuncPtr_stack_region_get_capacity)(StackRegion *region);
typedef size_t (*FuncPtr_stack_region_get_size)(StackRegion *region);
typedef size_t (*FuncPtr_stack_region_get_count)(StackRegion *region);
typedef size_t *(*FuncPtr_stack_region_get_count_ref)(StackRegion *region);

typedef StackRegionFrame (*FuncPtr_stack_region_peek)(StackRegion *stack, RegionError *error);
typedef StackRegionFrame (*FuncPtr_stack_region_peek_at)(StackRegion *stack, size_t index, RegionError *error);

typedef StackRegionFrame (*FuncPtr_stack_region_pop)(StackRegion *stack, RegionError *error);
typedef int *(*FuncPtr_stack_region_pop_int)(StackRegion *stack, RegionError *error);
typedef float *(*FuncPtr_stack_region_pop_float)(StackRegion *stack, RegionError *error);
typedef double *(*FuncPtr_stack_region_pop_double)(StackRegion *stack, RegionError *error);
typedef char *(*FuncPtr_stack_region_pop_char)(StackRegion *stack, RegionError *error);

typedef StackRegionFrame (*FuncPtr_stack_region_push)(StackRegion **stack, size_t size, RegionError *error);
typedef int *(*FuncPtr_stack_region_push_int)(StackRegion **stack, int value, RegionError *error);
typedef float *(*FuncPtr_stack_region_push_float)(StackRegion **stack, float value, RegionError *error);
typedef double *(*FuncPtr_stack_region_push_double)(StackRegion **stack, double value, RegionError *error);
typedef char *(*FuncPtr_stack_region_push_char)(StackRegion **stack, char value, RegionError *error);

typedef void (*FuncPtr_stack_region_swap)(StackRegion *stack, RegionError *error);
typedef void (*FuncPtr_stack_region_free)(StackRegion **stack);

typedef void (*FuncPtr_region_error_print_to)(REGION_FILE *stream, RegionError error);
typedef void (*FuncPtr_region_error_print)(RegionError error);


typedef void (*FuncPtr__test_set_available_memory)(size_t value);
typedef void (*FuncPtr__test_set_default_memory)();
typedef size_t (*FuncPtr__test_get_available_memory)(size_t value);

typedef struct {
    FuncPtr_region_alloc               region_alloc;

    FuncPtr_region_get_capacity        region_get_capacity;
    FuncPtr_region_get_size            region_get_size;

    FuncPtr_region_clone               region_clone;
    FuncPtr_region_collect             region_collect;
    FuncPtr_region_free                region_free;
    FuncPtr_region_get_last_node       region_get_last_node;
    FuncPtr_region_merge               region_merge;
    FuncPtr_region_shrink_capacity     region_shrink_capacity;

    FuncPtr_region_push                region_push;
    FuncPtr_region_push_int            region_push_int;
    FuncPtr_region_push_float          region_push_float;
    FuncPtr_region_push_double         region_push_double;
    FuncPtr_region_push_char           region_push_char;

    FuncPtr_stack_region_alloc         stack_region_alloc;

    FuncPtr_stack_region_get_capacity  stack_region_get_capacity;
    FuncPtr_stack_region_get_size      stack_region_get_size;
    FuncPtr_stack_region_get_count     stack_region_get_count;
    FuncPtr_stack_region_get_count_ref stack_region_get_count_ref;

    FuncPtr_stack_region_free          stack_region_free;
    FuncPtr_stack_region_peek          stack_region_peek;
    FuncPtr_stack_region_peek_at       stack_region_peek_at;
    FuncPtr_stack_region_swap          stack_region_swap;
    
    FuncPtr_stack_region_push          stack_region_push;
    FuncPtr_stack_region_push_int      stack_region_push_int;
    FuncPtr_stack_region_push_float    stack_region_push_float;
    FuncPtr_stack_region_push_double   stack_region_push_double;
    FuncPtr_stack_region_push_char     stack_region_push_char;

    FuncPtr_stack_region_pop           stack_region_pop;
    FuncPtr_stack_region_pop_int       stack_region_pop_int;
    FuncPtr_stack_region_pop_float     stack_region_pop_float;
    FuncPtr_stack_region_pop_double    stack_region_pop_double;
    FuncPtr_stack_region_pop_char      stack_region_pop_char;

    FuncPtr_region_error_print_to      region_error_print_to;
    FuncPtr_region_error_print         region_error_print;

    FuncPtr__test_set_available_memory test_set_available_memory;
    FuncPtr__test_set_default_memory   test_set_default_memory;
} RegionAPI;

RegionAPI *rt_try_get_region_api_handle(void *handle);
