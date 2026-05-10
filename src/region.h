#ifndef REGION_NO_STDIO
#include <stdio.h>
#define REGION_FILE FILE
#define REGION_STDIN stdin
#define REGION_STDOUT stdout
#define REGION_STDERR stderr
#define REGION_PRINTF(...) printf(__VA_ARGS__)
#define REGION_FPRINTF(...) fprintf(__VA_ARGS__)
#define REGION_SPRINTF(...) sprintf(__VA_ARGS__)
#endif // REGION_NO_STDIO

#ifndef REGION_NO_STDINT
#include <stdint.h>
#define REGION_SIZE_MAX __SIZE_MAX__
#define REGION_UINT8 uint8_t
#endif // REGION_NO_STDINT

#ifndef REGION_NO_STRING
#include <string.h>
#define REGION_STRLEN strlen
#define REGION_STRNCPY strncpy
#define REGION_STRCPY strcpy
#define REGION_MEMCPY memcpy
#endif // REGION_NO_STRING

#ifndef REGION_NO_STDLIB
#include <stdlib.h>
#define REGION_MALLOC malloc
#define REGION_FREE free
#endif // REGION_NO_STDLIB

#ifndef REGION_NO_ASSERT
#include <assert.h>
#define REGION_ASSERT assert
#endif // REGION_NO_ASSERT

#ifndef REGION_NO_STDBOOL
#include <stdbool.h>
#define REGION_BOOL bool
#define REGION_BOOL_TRUE true
#define REGION_BOOL_FALSE false
#endif // REGION_NO_STDBOOL

#if defined(_WIN32)
    #define REGION_API __declspec(dllexport)
#else
    #define REGION_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
  #define REGION_EXTERN_C_BEGIN extern "C" {
  #define REGION_EXTERN_C_END }
#else
  #define REGION_EXTERN_C_BEGIN
  #define REGION_EXTERN_C_END
#endif

typedef struct __Region Region;

typedef struct __StackRegion StackRegion;

typedef struct _StackRegionFrame {
    void *data;
    size_t size;
} StackRegionFrame;

typedef enum {
    REGION_RESET_OPTION_SOFT = 0,
    REGION_RESET_OPTION_HARD = 1,
} RegionResetOption;

typedef enum {
    REGION_MERGE_OPTION_DEFAULT = 0,
    REGION_MERGE_OPTION_CONDENSE
} RegionMergeOption;

typedef enum {
    REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT = 0,
    REGION_SHRINK_CAPACITY_OPTION_ALL
} RegionShrinkCapacityOption;

typedef enum {
    REGION_GET_LAST_NODE_OPTION_DEFAULT = 0,
    REGION_GET_LAST_NODE_OPTION_NON_EMPTY
} RegionGetLastNodeOption;

// ----- DATA STRUCTS FOR ERRORS -----

typedef enum {
    REGION_ERROR_CODE_NO_ERROR = 0,

    // general
    REGION_ERROR_CODE_EINVAL,
    REGION_ERROR_CODE_ENOMEM,

    // region_alloc
    REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY,  // The `Capacity` argument equals to `__SIZE_MAX__`.
    REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION,   // Failed to allocate the `Region` struct.
    REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, // Failed to allocate `capacity` bytes into the `Region->data` field.

    // region_push
    REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE,    // The `size` argument equals to `__SIZE_MAX__`.
    REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION, // Failed to allocate the `Region` struct for a new item.

    // region_push_int
    REGION_ERROR_CODE_ENOMEM_REGION_PUSH_INT_MALLOC_REGION, // Failed to allocate the `Region` struct for a new item.

    // region_push_float
    REGION_ERROR_CODE_ENOMEM_REGION_PUSH_FLOAT_MALLOC_REGION, // Failed to allocate the `Region` struct for a new item.

    // region_push_double
    REGION_ERROR_CODE_ENOMEM_REGION_PUSH_DOUBLE_MALLOC_REGION, // Failed to allocate the `Region` struct for a new item.

    // region_push_char
    REGION_ERROR_CODE_ENOMEM_REGION_PUSH_CHAR_MALLOC_REGION, // Failed to allocate the `Region` struct for a new item.

    // region_shrink_capacity
    REGION_ERROR_CODE_ENOMEM_REGION_SHRINK_CAPACITY_MALLOC,    // Failed to allocate memory for the `data` field.

    // region_collect
    REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_COLLECTED_SIZE, // The pointer to the `collected_size` argument equals to `NULL`.
    REGION_ERROR_CODE_ENOMEM_REGION_COLLECT_MALLOC_COLLECTION, // Failed to allocate memory for the array collection.

    // region_clone
    REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_ROOT, // Failed to allocate the root region provided as an argument.
    REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_NODE, // Failed to allocate a node from the source region.

    // region_merge
    REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_COLLECTION, // Failed to allocate a temporary buffer to store nodes' references.
    REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_REGION,     // Failed to allocate a region with combined capacity.

    // stack_region_alloc
    REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY,  // The value of `capacity` is too large.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_REGION,   // Failed to allocate the `StackRegion` struct.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_CAPACITY, // Failed to allocate `capacity` bytes into the `StackRegion->data` field.

    // stack_region_push
    REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_LARGE_SIZE,      // The `size` argument equals to `__SIZE_MAX__`.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION,   // Failed to allocate the `Region` struct for a new item.

    // stack_region_push_int
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_INT_MALLOC_REGION,   // Failed to allocate the `StackRegion` struct for a new item.

    // stack_region_push_float
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_FLOAT_MALLOC_REGION,   // Failed to allocate the `StackRegion` struct for a new item.

    // stack_region_push_double.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_DOUBLE_MALLOC_REGION,   // Failed to allocate the `StackRegion` struct for a new item.

    // stack_region_push_char
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_CHAR_MALLOC_REGION,

    // stack_region_peek
    REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_CORRUPTED_DATA, // The `data` field of the stack is corrupted 

    // stack_region_pop_int
    REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_INT_INVALID_FRAME,   // The last frame's size is not equal to sizeof(int)

    // stack_region_pop_float
    REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_FLOAT_INVALID_FRAME,   // The last frame's size is not equal to sizeof(float)

    // stack_region_pop_double
    REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_DOUBLE_INVALID_FRAME,   // The last frame's size is not equal to sizeof(double)

    // stack_region_pop_char
    REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_CHAR_INVALID_FRAME,

    // stack_region_swap
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_SWAP_MALLOC_TEMPORARY_BUFFER, // Failed to allocate a temporary buffer to which the swapped data gets copied.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_SWAP_PUSH_LAST,               // Failed to re-push the last frame as a previous one.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_SWAP_PUSH_PREV,               // Failed to re-push the previous frame as a last one.

} RegionErrorCode;

typedef struct {
    int line;
    const char *file_name;
    const char *func_name;
} RegionLocation;

typedef enum {
    REGION_ERROR_TYPE_NONE, 
    REGION_ERROR_TYPE_INVALID_ARGUMENT, 
    REGION_ERROR_TYPE_NO_MEMORY,
    REGION_ERROR_TYPE_UNKNOWN, 
} RegionErrorType;

typedef enum {
    REGION_ERROR_CLASS_REGION,
    REGION_ERROR_CLASS_STACK_REGION
} RegionErrorClass;

typedef enum {
    REGION_ERROR_FUNCTION_ALLOC,

    REGION_ERROR_FUNCTION_PUSH,
    REGION_ERROR_FUNCTION_PUSH_INT,
    REGION_ERROR_FUNCTION_PUSH_FLOAT,
    REGION_ERROR_FUNCTION_PUSH_DOUBLE,
    REGION_ERROR_FUNCTION_PUSH_CHAR,

    REGION_ERROR_FUNCTION_SHRINK_CAPACITY,

    REGION_ERROR_FUNCTION_COLLECT,

    REGION_ERROR_FUNCTION_CLONE,

    REGION_ERROR_FUNCTION_MERGE,

    REGION_ERROR_FUNCTION_PEEK,
    REGION_ERROR_FUNCTION_PEEK_AT,

    REGION_ERROR_FUNCTION_POP,
    REGION_ERROR_FUNCTION_POP_INT,
    REGION_ERROR_FUNCTION_POP_FLOAT,
    REGION_ERROR_FUNCTION_POP_DOUBLE,
    REGION_ERROR_FUNCTION_POP_CHAR,

    REGION_ERROR_FUNCTION_SWAP
} RegionErrorFunction;

typedef enum {
    REGION_ERROR_MESSAGE_ARG_LARGE_CAPACITY,
    REGION_ERROR_MESSAGE_ARG_LARGE_SIZE,
    REGION_ERROR_MESSAGE_ARG_SMALL_SIZE,

    REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION,
    REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_DATA,

    REGION_ERROR_MESSAGE_MALLOC_FAILURE_STACK_REGION,
    REGION_ERROR_MESSAGE_MALLOC_FAILURE_STACK_REGION_DATA,

    REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_ARRAY,
    REGION_ERROR_MESSAGE_MALLOC_FAILURE_TEMPORARY_BUFFER,

    REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA
} RegionErrorMessage;

typedef struct {
    RegionLocation location;
    RegionErrorCode code; // obsolete, has to be removed

    RegionErrorType type;
    RegionErrorClass function_class;
    RegionErrorFunction function;
    RegionErrorMessage message;
} RegionError;

#define REGION_SET_ERROR(error, error_code) if ((error)) (error)->code = (error_code);        
#define REGION_GET_CURRENT_FILE_LOCATION (RegionLocation){.file_name = __FILE__, .line = __LINE__, .func_name = __func__}
#define REGION_ERROR_INIT_LOCATION(error) (error)->location = REGION_GET_CURRENT_FILE_LOCATION          
#define REGION_ERROR_INIT (RegionError){.code = 0, .location.file_name = __FILE__, .location.line = __LINE__, .location.func_name = __func__}

#define REGION_NO_ERROR(error) (error).code == REGION_ERROR_CODE_NO_ERROR
#define REGION_ERROR(error)    (error).code != REGION_ERROR_CODE_NO_ERROR

REGION_EXTERN_C_BEGIN

// Region
REGION_API Region *region_alloc(size_t capacity, RegionError *error);

REGION_API size_t region_get_capacity(Region *region);
REGION_API size_t region_get_size(Region *region);

REGION_API Region *region_clone(Region *region, RegionError *error);
REGION_API Region *region_merge(Region *region, RegionMergeOption option, RegionError *error);
REGION_API Region *region_get_last_node(Region *region, RegionGetLastNodeOption option, RegionError *error);
REGION_API Region **region_collect(Region *region, size_t *collected_size, RegionError *error);

REGION_API void *region_push(Region **region, size_t size, RegionError *error);
REGION_API int *region_push_int(Region **region, int value, RegionError *error);
REGION_API float *region_push_float(Region **region, float value, RegionError *error);
REGION_API double *region_push_double(Region **region, double value, RegionError *error);
REGION_API char *region_push_char(Region **region, char value, RegionError *error);

REGION_API void region_reset(Region *region, RegionResetOption option);
REGION_API void region_free(Region **region);
REGION_API void region_shrink_capacity(Region *region, RegionShrinkCapacityOption option, RegionError *error);

// Stack Region
REGION_API StackRegion *stack_region_alloc(size_t capacity, RegionError *error);

REGION_API size_t stack_region_get_capacity(StackRegion *region);
REGION_API size_t stack_region_get_size(StackRegion *region);
REGION_API size_t stack_region_get_count(StackRegion *region);

REGION_API StackRegionFrame stack_region_push(StackRegion **stack, size_t size, RegionError *error);
REGION_API int *stack_region_push_int(StackRegion **stack, int value, RegionError *error);
REGION_API float *stack_region_push_float(StackRegion **stack, float value, RegionError *error);
REGION_API double *stack_region_push_double(StackRegion **stack, double value, RegionError *error);
REGION_API char *stack_region_push_char(StackRegion **stack, char value, RegionError *error);

REGION_API StackRegionFrame stack_region_peek(StackRegion *stack, RegionError *error);
REGION_API StackRegionFrame stack_region_peek_at(StackRegion *stack, size_t index, RegionError *error);
REGION_API StackRegionFrame stack_region_pop(StackRegion *stack, RegionError *error);

REGION_API int *stack_region_pop_int(StackRegion *stack, RegionError *error);
REGION_API float *stack_region_pop_float(StackRegion *stack, RegionError *error);
REGION_API double *stack_region_pop_double(StackRegion *stack, RegionError *error);
REGION_API char *stack_region_pop_char(StackRegion *stack, RegionError *error);

REGION_API void stack_region_swap(StackRegion *stack, RegionError *error);
REGION_API void stack_region_free(StackRegion **stack);

// Error
REGION_API void region_error_print_to(REGION_FILE *stream, RegionError error);
REGION_API void region_error_print(RegionError error);

#define REGION_IS_EMPTY(region)\
    region_get_capacity(region) == 0 && region_get_size(region) == 0;

// ----- * -----

#ifdef REGION_IMPLEMENTATION

#define REGION_CORE_FIELDS \
    size_t capacity;       \
    size_t size;           \
    char *data;            \
    struct __Region *next; \

typedef struct __Region      { REGION_CORE_FIELDS } Region;
typedef struct __StackRegion { REGION_CORE_FIELDS } StackRegion;

#define STACK_REGION_CACHE_COUNT_SIZE sizeof(size_t)

#define STACK_REGION_GET_COUNT_REF(stack) (size_t *)((stack)->data - STACK_REGION_CACHE_COUNT_SIZE)
#define STACK_REGION_FRAME_EMPTY (StackRegionFrame){ .data = NULL, .size = 0}
#define STACK_REGION_FRAME_IS_EMPTY(frame) ((frame).data == NULL && (frame).size == 0)

/**
 * @brief Returns a newly allocated region.
 *
 * @param capacity Number of bytes for region data
 * @param error Optional error output
 * 
 * @return Pointer to allocated `Region` struct. Ownership is passed to the caller.
 * Must be freed with `region_free`
 * 
 * @warning Memory leak if returned pointer is not freed
 * 
 * @note Region fields are initialized to
 *  `0`    - for `numeric` types..
 *  `NULL` - for pointers.
 * 
 * @note If `capacity` equals to zero, then `data` gets initialized to `NULL`
 * and other functions that modify `data` will try to allocate memory to this
 * field. It's recommended to preallocate enough memory to avoid extra nodes
 * creation when new data gets pushed.
 * 
 * @note In case of errors, the following opcodes may be returned:
 *  `REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY` - `capacity` is too large
 *  `REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION` - failed to allocate `Region` struct
 *  `REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY` - failed to allocate `capacity` bytes for `Region` data.
 * 
 * @code{.c}
 * Region *region = region_alloc(4 *sizeof(int), NULL);
 * if (!region) {
 *      // error handling...
 * }
 * region_free(&region);
 * @endcode
 */
Region *region_alloc(size_t capacity, RegionError *error)
{
    if (capacity > REGION_SIZE_MAX - sizeof(Region)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY);
        return NULL;
    }

    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION);
        return NULL;
    }
    
    region->capacity = capacity;
    region->size = 0;
    region->next = NULL;

    if (capacity == 0) {
        region->data = NULL;
        return region;
    }

    region->data = (char *)REGION_MALLOC(sizeof(char) * capacity);
    
    if (!region->data) {
        REGION_FREE(region);
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY);
        return NULL;
    }

    REGION_MEMCPY(region->data, "\0", region->capacity);
    
    return region;
}

size_t region_get_capacity(Region *region) { if (!region) return 0; return region->capacity; }
size_t region_get_size(Region *region) { if (!region) return 0; return region->size; }

void region_free(Region **region)
{
    if (!region || !*region)
        return;

    Region *current = *region;

    while (current) {
        Region *next = current->next;

        if (current->data)
            REGION_FREE(current->data);

        REGION_FREE(current);

        current = next;
    }

    *region = NULL;
}

void *region_push(Region **region, size_t size, RegionError *error)
{
    if (!region) return NULL;

    if (size == 0) return NULL;

    if (size > REGION_SIZE_MAX - sizeof(Region)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE);
        return NULL;
    }

    if (!(*region)) {
        *region = region_alloc(size, NULL);
        if (!(*region)) REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION);
        return NULL;
    }

    Region *current = *region;

    if (!current->data) {
        current->data = (char *)REGION_MALLOC(size * sizeof(char));

        if (!current->data) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION);
            return NULL;
        }

        current->capacity = size * sizeof(char);
    }

    while (current->size + size > current->capacity) {
        if (current->next) {
            current = current->next;
            continue;
        }
        
        current->next = region_alloc(current->capacity * 2 + size, NULL);

        if (!current->next) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION);
            return NULL;
        }

        current = current->next;
        break;
    }
   
    void *result = current->data + current->size;

    current->size += size;

    return result;
}

int *region_push_int(Region **region, int value, RegionError *error)
{
    if (!region) return NULL;

    RegionError local_error = {0};

    int *result = (int *)region_push(region, sizeof(int), &local_error);

    if (result) {
        *result = value;
        return result;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_INT_MALLOC_REGION); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return NULL;
}

float *region_push_float(Region **region, float value, RegionError *error)
{
    if (!region) return NULL;

    RegionError local_error = {0};

    float *result = (float *)region_push(region, sizeof(float), &local_error);

    if (result) {
        *result = value;
        return result;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_FLOAT_MALLOC_REGION); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return NULL;
}

double *region_push_double(Region **region, double value, RegionError *error)
{
    if (!region) return NULL;
    
    RegionError local_error = {0};
    
    double *result = (double *)region_push(region, sizeof(double), &local_error);
    
    if (result) {
        *result = value;
        return result;
    }
    
    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION:
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_DOUBLE_MALLOC_REGION); break;
        
        default:
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }
    
    return NULL;
}

char *region_push_char(Region **region, char value, RegionError *error)
{
    if (!region) return NULL;

    RegionError local_error = {0};

    char *result = (char *)region_push(region, sizeof(char), &local_error);

    if (result) {
        *result = value;
        return result;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_CHAR_MALLOC_REGION); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return NULL;
}

void region_reset(Region *region, RegionResetOption option)
{
    if (!region) return;

    if (option == REGION_RESET_OPTION_HARD) {
        Region *second = region->next;
        region_free(&second);
    }

    for (Region *i = region; i; i = i->next) {
        i->size = 0;
    }
}

void __region_shrink_capacity_helper(Region *region, RegionError *error)
{
    if (region->size == region->capacity || region->size == 0) return;

    char *shrinked_buffer = (char *)REGION_MALLOC(sizeof(char) * region->size);

    if (!shrinked_buffer) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_SHRINK_CAPACITY_MALLOC);
        return;
    }

    REGION_MEMCPY(shrinked_buffer, region->data, region->size);
    REGION_FREE(region->data);

    region->capacity = region->size;
    region->data = shrinked_buffer;

    return;
}

void region_shrink_capacity(Region *region, RegionShrinkCapacityOption option, RegionError *error)
{
    if (!region) return;

    if (option == REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT) {
        __region_shrink_capacity_helper(region, error);
        return;
    }

    for (Region *t = region; t; t = t->next) {
        __region_shrink_capacity_helper(t, error);
            
        if (error->code != REGION_ERROR_CODE_NO_ERROR) return;
    }
}

Region **region_collect(Region *region, size_t *collected_size, RegionError *error)
{
    if (!region) return NULL;

    if (!collected_size) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_COLLECTED_SIZE);
        return NULL;
    }

    size_t nodes_count = 0;

    for (Region *t = region; t; t = t->next) {
        nodes_count++;
    }

    Region **collection = (Region **)REGION_MALLOC(sizeof(Region *) * nodes_count);

    if (!collection) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_COLLECT_MALLOC_COLLECTION);
        return NULL;
    }

    *collected_size = nodes_count;

    nodes_count = 0;

    for (Region *t = region; t; t = t->next) {
        collection[nodes_count] = t;
        nodes_count++;
    }

    return collection;
}

Region *region_clone(Region *region, RegionError *error)
{
    if (!region) return NULL;

    Region *clone = region_alloc(region->capacity, NULL);

    if (!clone) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_ROOT);
        return NULL;
    }

    clone->capacity = region->capacity;
    clone->size = region->size;

    if (region->data) REGION_MEMCPY(clone->data, region->data, region->size);

    Region *clone_i = clone;

    for (Region *t = region->next; t; t = t->next) {
        Region *node = region_alloc(t->capacity, NULL);

        if (!node) {
            region_free(&clone);
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_NODE);
            return NULL;
        }

        node->capacity = t->capacity;
        node->size = t->size;

        if (t->data) REGION_MEMCPY(node->data, t->data, t->size);

        clone_i->next = node;
        clone_i = clone_i->next;
    }

    return clone;
}

Region *region_merge(Region *region, RegionMergeOption option, RegionError *error)
{
    if (!region) return NULL;

    size_t collected_size = 0;
    Region **collection = region_collect(region, &collected_size, NULL);

    if (!collection) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_COLLECTION);
        return NULL;
    }

    size_t merged_capacity = 0;

    for (size_t i = 0; i < collected_size; i++) {
        if (option == REGION_MERGE_OPTION_DEFAULT) {
            merged_capacity += collection[i]->capacity;
        } else {
            merged_capacity += collection[i]->size;
        }
    }

    if (merged_capacity == 0) {
        REGION_FREE(collection);
        return region_alloc(0, NULL);
    }

    Region *merged_region = region_alloc(merged_capacity, NULL);

    if (!merged_region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_REGION);
        REGION_FREE(collection);
        return NULL;
    }

    size_t characters_read = 0;

    for (size_t i = 0; i < collected_size; i++) {
        size_t to_read = option == REGION_MERGE_OPTION_DEFAULT ? collection[i]->capacity : collection[i]->size;

        if (to_read == 0) continue;
  
        REGION_MEMCPY(merged_region->data + characters_read, collection[i]->data, to_read);

        characters_read += to_read;   
    }

    REGION_FREE(collection);
    return merged_region;
}

Region *region_get_last_node(Region *region, RegionGetLastNodeOption option, RegionError *error)
{
    if (!region) return NULL;

    Region *last_node = region;

    while (last_node->next != NULL) {
        bool predicate = option == REGION_GET_LAST_NODE_OPTION_NON_EMPTY ? (last_node->next)->size != 0 : true;

        if (!predicate) break;

        last_node = last_node->next;
    }

    return last_node;
}

StackRegion *stack_region_alloc(size_t capacity, RegionError *error)
{
    if (capacity > REGION_SIZE_MAX - sizeof(StackRegion)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY);
        return NULL;
    }
    
    RegionError local_error = {0};
    Region *stack = region_alloc(capacity + STACK_REGION_CACHE_COUNT_SIZE, &local_error);
    
    if (REGION_NO_ERROR(local_error)) {
        size_t *count = (size_t *)stack->data;
        *count = 0;
        stack->data += STACK_REGION_CACHE_COUNT_SIZE;
        return (StackRegion *)stack;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_REGION); break;

        case REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_CAPACITY); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }
    
    return NULL;
}

size_t stack_region_get_capacity(StackRegion *region) { if (!region) return 0; return region->capacity; }
size_t stack_region_get_size(StackRegion *region) { if (!region) return 0; return region->size; }

size_t stack_region_get_count(StackRegion *region)
{
    if (!region || !region->data) return 0;

    return *(size_t *)(region->data - STACK_REGION_CACHE_COUNT_SIZE);    
}

StackRegionFrame stack_region_push(StackRegion **stack, size_t size, RegionError *error)
{
    if (!stack) return STACK_REGION_FRAME_EMPTY;

    if (size == 0) return STACK_REGION_FRAME_EMPTY;
    
    if (size + sizeof(size_t) < size) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_LARGE_SIZE);
        return STACK_REGION_FRAME_EMPTY;
    }

    RegionError local_error = {0};

    void *frame_data = region_push((Region **)stack, size + sizeof(size_t), &local_error);

    if (REGION_NO_ERROR(local_error)) {
        *STACK_REGION_GET_COUNT_REF(*stack) += 1;
        *(size_t *)(frame_data + size) = size;

        StackRegionFrame frame = {0};
        frame.data = frame_data;
        frame.size = size;

        return frame;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION); break;

        default: 
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return STACK_REGION_FRAME_EMPTY;
}

// TODO: Remove code reduplication by moving the logic from the stack_region_push_<type> functions
// into a separate helper function called __stack_region_push_numeric_type
int *stack_region_push_int(StackRegion **stack, int value, RegionError *error)
{
    if (!stack) return NULL;

    RegionError local_error = REGION_ERROR_INIT;

    StackRegionFrame result = stack_region_push(stack, sizeof(int), &local_error);

    if (REGION_NO_ERROR(local_error)) {
        *(int *)result.data = value;
        return (int *)result.data;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_INT_MALLOC_REGION); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return NULL;
}

float *stack_region_push_float(StackRegion **stack, float value, RegionError *error)
{
    if (!stack) return NULL;

    RegionError local_error = REGION_ERROR_INIT;

    StackRegionFrame result = stack_region_push(stack, sizeof(float), &local_error);

    if (REGION_NO_ERROR(local_error)) {
        *(float *)result.data = value;
        return (float *)result.data;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_FLOAT_MALLOC_REGION); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return NULL;
}

double *stack_region_push_double(StackRegion **stack, double value, RegionError *error)
{
    if (!stack)  return NULL;

    RegionError local_error = REGION_ERROR_INIT;

    StackRegionFrame result = stack_region_push(stack, sizeof(double), &local_error);

    if (REGION_NO_ERROR(local_error)) {
        *(double *)result.data = value;
        return (double *)result.data;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_DOUBLE_MALLOC_REGION); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return NULL;
}

char *stack_region_push_char(StackRegion **stack, char value, RegionError *error)
{
    if (!stack) return NULL;

    RegionError local_error = REGION_ERROR_INIT;

    StackRegionFrame result = stack_region_push(stack, sizeof(char), &local_error);

    if (REGION_NO_ERROR(local_error)) {
        *(char *)result.data = value;
        return (char *)result.data;
    }

    switch (local_error.code) {
        case REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_CHAR_MALLOC_REGION); break;
        
        default:
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM); break;
    }

    return NULL;
}

StackRegionFrame stack_region_peek(StackRegion *stack, RegionError *error)
{
    if (!stack) return STACK_REGION_FRAME_EMPTY;

    if (stack_region_get_count(stack) == 0) return STACK_REGION_FRAME_EMPTY;

    Region *last_node = region_get_last_node((Region *)stack, REGION_GET_LAST_NODE_OPTION_NON_EMPTY, NULL);

    if (!last_node || !(last_node->data)) return STACK_REGION_FRAME_EMPTY;

    if (last_node->size <= STACK_REGION_CACHE_COUNT_SIZE) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_CORRUPTED_DATA);
        return STACK_REGION_FRAME_EMPTY;
    }

    if (last_node->size - STACK_REGION_CACHE_COUNT_SIZE * stack_region_get_count(stack) < stack_region_get_count(stack)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_CORRUPTED_DATA);
        return STACK_REGION_FRAME_EMPTY;
    }

    void *last_frame_end = last_node->data + last_node->size;

    size_t last_frame_size = *(size_t *)(last_frame_end - sizeof(size_t));

    void *last_frame_start = last_frame_end - sizeof(size_t) - last_frame_size;

    StackRegionFrame frame = {
        .data = last_frame_start,
        .size = last_frame_size
    };

    return frame;
}

StackRegionFrame stack_region_peek_at(StackRegion *stack, size_t index, RegionError *error)
{
    if (!stack) return STACK_REGION_FRAME_EMPTY;

    if (stack_region_get_count(stack) == 0 || 
        stack_region_get_count(stack) <= index) return STACK_REGION_FRAME_EMPTY;

    size_t last_node_index = 0;
    Region *last_node = (Region *)stack;

    while (last_node->next != NULL && (last_node->next)->size > 0) {
        last_node = last_node->next;
        last_node_index++;
    } if (last_node_index != 0) last_node_index--;

    size_t bytes_to_read = last_node->size;

    for (size_t i = 0; i <= index; i++) {
        void *indexed_frame_end = last_node->data + bytes_to_read;

        size_t indexed_frame_size = *(size_t *)(indexed_frame_end - sizeof(size_t));

        bytes_to_read -= indexed_frame_size + sizeof(size_t);

        if (bytes_to_read == 0) {
            if (i >= index) break;

            last_node = (Region *)stack;
            for (size_t j = 0; j < last_node_index; j++) {
                last_node = last_node->next;
            }
            
            bytes_to_read = last_node->size;
            
            if (last_node_index == 0) last_node_index = 0;
            else last_node_index--;
        }
    }

    StackRegionFrame frame = {
        .data = last_node->data + bytes_to_read,
        .size = *(size_t *)(last_node->data + bytes_to_read - sizeof(size_t))
    };

    return frame;
}

StackRegionFrame stack_region_pop(StackRegion *stack, RegionError *error)
{
    if (!stack) return STACK_REGION_FRAME_EMPTY;

    if (stack_region_get_count(stack) == 0) return STACK_REGION_FRAME_EMPTY;

    Region *last_node = (Region *)stack;

    while (last_node->next && last_node->next->size != 0) {
        last_node = last_node->next;
    }

    if (sizeof(size_t) >= last_node->size) {
        return STACK_REGION_FRAME_EMPTY;
    }

    void *last_frame_end = last_node->data + last_node->size;

    size_t last_frame_size = *(size_t *)(last_frame_end - sizeof(size_t));

    void *last_frame_start = last_frame_end - sizeof(size_t) - last_frame_size;
    
    (*STACK_REGION_GET_COUNT_REF(stack))--;

    last_node->size -= (last_frame_size + sizeof(size_t));

    StackRegionFrame frame = {
        .data = last_frame_start,
        .size = last_frame_size
    };

    return frame;
}

int *stack_region_pop_int(StackRegion *stack, RegionError *error)
{
    if (!stack) return NULL;

    if (stack_region_peek(stack, NULL).size != sizeof(int)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_INT_INVALID_FRAME);
        return NULL;
    }

    StackRegionFrame frame = stack_region_pop(stack, NULL);

    return (int *)(frame.data);
}

float *stack_region_pop_float(StackRegion *stack, RegionError *error)
{
    if (!stack) return NULL;
    
    if (stack_region_peek(stack, NULL).size != sizeof(float)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_FLOAT_INVALID_FRAME);
        return NULL;
    }

    return (float *)stack_region_pop(stack, NULL).data;
}

double *stack_region_pop_double(StackRegion *stack, RegionError *error)
{
    if (!stack) return NULL;

    if (stack_region_peek(stack, NULL).size != sizeof(double)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_DOUBLE_INVALID_FRAME);
        return NULL;
    }

    return (double *)stack_region_pop(stack, NULL).data;
}

char *stack_region_pop_char(StackRegion *stack, RegionError *error)
{
    if (!stack) return NULL;

    if (stack_region_peek(stack, NULL).size != sizeof(char)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_CHAR_INVALID_FRAME);
        return NULL;
    }

    return (char *)stack_region_pop(stack, NULL).data;
}

void stack_region_swap(StackRegion *stack, RegionError *error)
{
    if (!stack) return;

    if (stack_region_get_count(stack) < 2) return;
  
    StackRegionFrame last = stack_region_pop(stack, error);
    StackRegionFrame prev = stack_region_pop(stack, error);

    // TODO: Reimplement buffering by only copying the smaller 
    // frame and rewrite its contents with the bigger one to 
    // reduce the amount of memory consumed by this algorithm
    void *temporary_buffer = REGION_MALLOC(last.size + prev.size);

    if (!temporary_buffer) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_SWAP_MALLOC_TEMPORARY_BUFFER);
        return;
    }

    REGION_MEMCPY(temporary_buffer, last.data, last.size);
    REGION_MEMCPY(temporary_buffer + last.size, prev.data, prev.size);

    StackRegionFrame repushed_last = stack_region_push(&stack, last.size, error);

    if (STACK_REGION_FRAME_IS_EMPTY(repushed_last)) {
        REGION_FREE(temporary_buffer);
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_SWAP_PUSH_LAST);
        return;
    }
    
    StackRegionFrame repushed_prev = stack_region_push(&stack, prev.size, error);

    if (STACK_REGION_FRAME_IS_EMPTY(repushed_prev)) {
        stack_region_pop(stack, NULL);
        REGION_FREE(temporary_buffer);
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_STACK_REGION_SWAP_PUSH_LAST);
        return;
    }

    REGION_MEMCPY(repushed_last.data, temporary_buffer, last.size);
    REGION_MEMCPY(repushed_prev.data, temporary_buffer + last.size, prev.size);
    REGION_FREE(temporary_buffer);
}

void stack_region_free(StackRegion **stack)
{
    if (!stack || !(*stack)) return;

    (*stack)->data -= STACK_REGION_CACHE_COUNT_SIZE;

    region_free((Region **)stack);
}

// TODO: Split the string convertions into separate functions
void region_error_print_to(REGION_FILE *stream, RegionError error)
{
    if (REGION_NO_ERROR(error)) {
        REGION_FPRINTF(stream, "[Region][Log]: No error\n");
        return;
    }

    char type_string[32];
    char class_string[32];
    char function_string[32];
    char message_string[128];

    switch (error.type) {
        case REGION_ERROR_TYPE_INVALID_ARGUMENT:
            REGION_SPRINTF(type_string, "%s: ", "Invalid argument"); break;
        case REGION_ERROR_TYPE_NO_MEMORY:
            REGION_SPRINTF(type_string, "%s: ", "No available memory"); break;
        default:
            REGION_SPRINTF(type_string, "%s: ", "Unknown error type"); break;
    }

    switch (error.function_class) {
        case REGION_ERROR_CLASS_REGION:
            REGION_SPRINTF(class_string, "%s_", "region"); break;
        case REGION_ERROR_CLASS_STACK_REGION:
            REGION_SPRINTF(class_string, "%s_", "stack_region"); break;
        default:
            REGION_SPRINTF(class_string, "%s_", "<unknown>"); break;
    }

    switch (error.function) {
        case REGION_ERROR_FUNCTION_ALLOC:
            REGION_SPRINTF(function_string, "%s: ", "alloc"); break;
        case REGION_ERROR_FUNCTION_CLONE:
            REGION_SPRINTF(function_string, "%s: ", "clone"); break;
        case REGION_ERROR_FUNCTION_COLLECT:
            REGION_SPRINTF(function_string, "%s: ", "collect"); break;
        case REGION_ERROR_FUNCTION_MERGE:
            REGION_SPRINTF(function_string, "%s: ", "merge"); break;
        case REGION_ERROR_FUNCTION_PEEK:
            REGION_SPRINTF(function_string, "%s: ", "peek"); break;
        case REGION_ERROR_FUNCTION_PEEK_AT:
            REGION_SPRINTF(function_string, "%s: ", "peek_at"); break;
        case REGION_ERROR_FUNCTION_POP:
            REGION_SPRINTF(function_string, "%s: ", "pop"); break;
        case REGION_ERROR_FUNCTION_POP_CHAR:
            REGION_SPRINTF(function_string, "%s: ", "pop_char"); break;
        case REGION_ERROR_FUNCTION_POP_DOUBLE:
            REGION_SPRINTF(function_string, "%s: ", "pop_double"); break;
        case REGION_ERROR_FUNCTION_POP_FLOAT:
            REGION_SPRINTF(function_string, "%s: ", "pop_float"); break;
        case REGION_ERROR_FUNCTION_POP_INT:
            REGION_SPRINTF(function_string, "%s: ", "pop_int"); break;
        case REGION_ERROR_FUNCTION_PUSH:
            REGION_SPRINTF(function_string, "%s: ", "push"); break;
        case REGION_ERROR_FUNCTION_PUSH_CHAR:
            REGION_SPRINTF(function_string, "%s: ", "push_char"); break;
        case REGION_ERROR_FUNCTION_PUSH_DOUBLE:
            REGION_SPRINTF(function_string, "%s: ", "push_double"); break;
        case REGION_ERROR_FUNCTION_PUSH_FLOAT:
            REGION_SPRINTF(function_string, "%s: ", "push_float"); break;
        case REGION_ERROR_FUNCTION_PUSH_INT:
            REGION_SPRINTF(function_string, "%s: ", "push_int"); break;
        case REGION_ERROR_FUNCTION_SHRINK_CAPACITY:
            REGION_SPRINTF(function_string, "%s: ", "shrink_capacity"); break;
        case REGION_ERROR_FUNCTION_SWAP:
            REGION_SPRINTF(function_string, "%s: ", "swap"); break;
        default:
            REGION_SPRINTF(function_string, "%s: ", "<unknown>"); break;
    }

    switch (error.message) {
        case REGION_ERROR_MESSAGE_ARG_LARGE_CAPACITY:
            REGION_SPRINTF(message_string, "%s.", "The value of `capacity` is too large. Cannot allocate memory"); break;
        case REGION_ERROR_MESSAGE_ARG_LARGE_SIZE:
            REGION_SPRINTF(message_string, "%s.", "The value of `size` is too large. Cannot allocate memory"); break;
        case REGION_ERROR_MESSAGE_ARG_SMALL_SIZE:
            REGION_SPRINTF(message_string, "%s.", "The value of `size` cannot be equal to `0`"); break;
        case REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA:
            REGION_SPRINTF(message_string, "%s.", "The stack frame's size or data has been corrupted or misinterpretted"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate a `Region` struct"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_ARRAY:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate a an array of `Region` structs"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_DATA:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate data for the `Region` struct"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_STACK_REGION:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate a `StackRegion` struct"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_STACK_REGION_DATA:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate data for the `Region` struct"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_TEMPORARY_BUFFER:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate a temporary buffer"); break;
        default: 
            REGION_SPRINTF(message_string, "%s.", "Unknown error message"); break;
    }

    REGION_FPRINTF(stream, "[Region][Error](\"%s\":%s():%d): %s%s%s%s\n", 
        error.location.file_name,
        error.location.func_name,
        error.location.line,
        
        type_string,
        class_string,
        function_string,
        message_string
    );
}

void region_error_print(RegionError error) { region_error_print_to(REGION_STDERR, error); }

#endif // REGION_IMPLEMENTATION
REGION_EXTERN_C_END
