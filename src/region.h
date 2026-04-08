#ifndef REGION_NO_STDIO
#include <stdio.h>
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
#define REGION_FREE free
    #ifndef REGION_TEST_IMPLEMENTATION
        #define REGION_MALLOC malloc
    #else
        #define REGION_MALLOC test_malloc
    #endif // REGION_TEST_IMPLEMENTATION
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

#ifdef __cplusplus
  #define REGION_EXTERN_C_BEGIN extern "C" {
  #define REGION_EXTERN_C_END }
#else
  #define REGION_EXTERN_C_BEGIN
  #define REGION_EXTERN_C_END
#endif

#define REGION_PRIVATE_CORE_FIELDS \
    size_t capacity;               \
    size_t size;                   \
    char *data;                    \
    struct __Region *next;         \

typedef struct __Region {
    REGION_PRIVATE_CORE_FIELDS
} Region;

typedef struct __StackRegion {
    REGION_PRIVATE_CORE_FIELDS
} StackRegion;

typedef enum {
    REGION_RESET_OPTION_SOFT = 0,
    REGION_RESET_OPTION_HARD = 1,
} RegionResetOption;

typedef enum {
    REGION_MERGE_OPTION_DEFAULT,
    REGION_MERGE_OPTION_CONDENSE
} RegionMergeOption;

typedef enum {
    REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT,
    REGION_SHRINK_CAPACITY_OPTION_ALL
} RegionShrinkCapacityOption;

#define REGION_TEST_AVAILABLE_MEMORY_DEFAULT 2048

#ifdef REGION_TEST_IMPLEMENTATION
    // Emulates the amount of memory a process can request from an OS
    static size_t __test_available_memory = REGION_TEST_AVAILABLE_MEMORY_DEFAULT;

    void __test_set_available_memory(size_t value) 
    {
        __test_available_memory = value;
    }

    size_t __test_get_available_memory()
    {
        return __test_available_memory;
    }

    void *test_malloc(size_t capacity)
    {
        if (capacity > __test_available_memory) return NULL;

        if (__test_available_memory - capacity > __test_available_memory) {
            __test_available_memory = 0;
            return NULL;
        }

        __test_available_memory -= capacity;

        return malloc(capacity);
    }
#endif // REGION_TEST_IMPLEMENTATION

// ----- DATA STRUCTS FOR ERRORS -----

typedef enum {
    REGION_ERROR_CODE_NO_ERROR = 0,

    // general
    REGION_ERROR_CODE_EINVAL,
    REGION_ERROR_CODE_ENOMEM,

    // __region_alloc
    REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY,  // The `Capacity` argument equals to zero.
    REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY,  // The `Capacity` argument equals to `__SIZE_MAX__`.
    REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION,   // Failed to allocate the `Region` struct.
    REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, // Failed to allocate `capacity` bytes into the `Region->data` field.

    // __region_push
    REGION_ERROR_CODE_EINVAL_REGION_PUSH_NO_REGION,     // The pointer to the `Region` struct equals to `NULL`.
    REGION_ERROR_CODE_EINVAL_REGION_PUSH_SMALL_SIZE,    // The `size` argument equals to zero.
    REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE,    // The `size` argument equals to `__SIZE_MAX__`.
    REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION, // Failed to allocate the `Region` struct for a new item.

    // __region_shrink_capacity
    REGION_ERROR_CODE_EINVAL_REGION_SHRINK_CAPACITY_NO_REGION, // The pointer to the `Region` struct equals to `NULL`.
    REGION_ERROR_CODE_ENOMEM_REGION_SHRINK_CAPACITY_MALLOC,    // Failed to allocate memory for the `data` field.

    // __region_collect
    REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_REGION,         // The pointer to the `Region` struct equals to `NULL`.
    REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_COLLECTED_SIZE, // The pointer to the `collected_size` argument equals to `NULL`.
    REGION_ERROR_CODE_ENOMEM_REGION_COLLECT_MALLOC_COLLECTION, // Failed to allocate memory for the array collection.

    // __region_clone
    REGION_ERROR_CODE_EINVAL_REGION_CLONE_NO_REGION,   // The pointer to the `Region` struct equals to `NULL`.
    REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_ROOT, // Failed to allocate the root region provided as an argument.
    REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_NODE, // Failed to allocate a node from the source region.

    // __region_merge
    REGION_ERROR_CODE_EINVAL_REGION_MERGE_NO_REGION,         // The pointer to the `Region` struct equals to `NULL`.
    REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_COLLECTION, // Failed to allocate a temporary buffer to store nodes' references.
    REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_REGION,     // Failed to allocate a region with combined capacity.

    // region_get_last_node
    REGION_ERROR_CODE_EINVAL_REGION_GET_LAST_NODE_NO_REGION,   // The pointer to the `Region` struct equals to `NULL`.

    // __stack_region_alloc
    REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_SMALL_CAPACITY,  // The value of `capacity` cannot equal to zero.
    REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY,  // The value of `capacity` is too large.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_REGION,   // Failed to allocate the `StackRegion` struct.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_CAPACITY, // Failed to allocate `capacity` bytes into the `StackRegion->data` field.

    // __stack_region_push
    REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_NO_STACK_REGION, // The pointer to the `Region` struct equals to `NULL`.
    REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_SMALL_SIZE,      // The `size` argument equals to zero.
    REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_LARGE_SIZE,      // The `size` argument equals to `__SIZE_MAX__`.
    REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION,   // Failed to allocate the `Region` struct for a new item.

    // __stack_region_peek
    REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_NO_STACK_REGION, // The pointer to the `StackRegion` struct equals to `NULL`.

    // __stack_region_pop
    REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_NO_STACK_REGION, // The pointer to the `Region` struct equals to `NULL`.

} RegionErrorCode;

static const char *region_error_code_as_strings[] = {
    "No error",

    // General
    "Invalid argument.",
    "No free space.",

    // __region_alloc
    "Invalid argument: The value of `capacity` cannot equal to zero.",
    "Invalid argument: The value of `capacity` is too large. Cannot allocate memory.",
    "No free space: Failed to allocate a `Region` struct.",
    "No free space: Failed to allocate `capacity` bytes into the `Region` struct.",

    // __region_push
    "Invalid argument: The value of `region` cannot equal to `NULL`.",
    "Invalid argument: The value of `size` cannot equal to zero.",
    "Invalid argument: The value of `size` is too large. Cannot allocate memory.",
    "No free space: Failed to allocate a `Region` struct for a new item.",

    // __region_shrink_capacity
    "Invalid argument: The value of `region` cannot equal to `NULL`.",
    "No free space: Failed to allocate a new shrinked buffer.",

    // __region_collect
    "Invalid argument: The value of `region` cannot equal to `NULL`.",
    "Invalid argument: The value of `collected_size` cannot equal to `NULL`.", 
    "No free space: Failed to allocate an array to collect all the nodes in the `region`.", 

    // __region_clone
    "Invalid argument: The value of `region` cannot equal to `NULL`.",
    "No free space: Failed to clone the `region` argument.",
    "No free space: Failed to clone a node from the `region` argument.",

    // __region_merge
    "Invalid argument: The value of `region` cannot equal to `NULL`.",
    "No free space: Failed to allocate a temporary buffer to store a collection of `region`'s nodes.",
    "No free space: Failed to allocate a region with the combined capacity",

    // region_get_last_node
    "Invalid argument: the value of `region` cannot equal to `NULL`",

    // __stack_region_alloc
    "Invalid argument: The value of `capacity` cannot equal to zero.",
    "Invalid argument: The value of `capacity` is too large. Cannot allocate memory.",
    "No free space: Failed to allocate a `StackRegion` struct.",
    "No free space: Failed to allocate `capacity` bytes into the `StackRegion` struct.",

    // __stack_region_push
    "Invalid argument: The value of `StackRegion` cannot equal to `NULL`.",
    "Invalid argument: The value of `size` cannot equal to zero.",
    "Invalid argument: The value of `size` is too large. Cannot allocate memory.",
    "No free space: Failed to allocate a `Region` struct for a new item.",

    // __stack_region_pop
    "Invalid argument: The value of `StackRegion` cannot equal to `NULL`."
};

typedef struct {
    int line;
    const char *file_name;
    const char *func_name;
} RegionLocation;

typedef struct {
    RegionLocation location;
    RegionErrorCode code;
} RegionError;

#define REGION_SET_ERROR(error, error_code, location)                \
    if ((error)) {                                                   \
        (error)->code = (error_code);                                \
        (error)->location.line = (location).line;                    \
        (error)->location.file_name = (location).file_name;          \
        (error)->location.func_name = (location).func_name;          \
    }                                                                \

#define REGION_LOG_ERROR_TO(error, out)                          \
    REGION_FPRINTF((out), "[Region][ERROR](\"%s:%d:%s\"): %s\n", \
        (error).location.file_name,                              \
        (error).location.line,                                   \
        (error).location.func_name,                              \
        region_error_code_as_strings[(error).code]);             \

#define REGION_LOG_ERROR(error) REGION_LOG_ERROR_TO((error), REGION_STDERR)
#define REGION_NO_ERROR(error) (error).code == REGION_ERROR_CODE_NO_ERROR
#define REGION_ERROR(error) !(REGION_NO_ERROR((error)))

REGION_EXTERN_C_BEGIN

// ----- FUNCTION DECLARATIONS (PRIVATE) -----

Region *__region_alloc(size_t capacity, RegionError *error, RegionLocation location);
void *__region_push(Region *region, size_t size, RegionError *error, RegionLocation location);
void __region_shrink_capacity(Region *region, RegionShrinkCapacityOption option, RegionError *error, RegionLocation location);
Region **__region_collect(Region *region, size_t *collected_size, RegionError *error, RegionLocation location);
Region *__region_clone(Region *region, RegionError *error, RegionLocation location);
Region *__region_merge(Region *region, RegionMergeOption option, RegionError *error, RegionLocation location);

// Stack Region
StackRegion *__stack_region_alloc(size_t capacity, RegionError *error, RegionLocation location);
void *__stack_region_push(StackRegion *stack, size_t size, RegionError *error, RegionLocation location);
void *__stack_region_peek(StackRegion *stack, RegionError *error, RegionLocation location);
void *__stack_region_pop(StackRegion *stack, RegionError *error, RegionLocation location);

// ----- PUBLIC API -----
void region_free(Region **region);
void stack_region_free(StackRegion **stack);
void region_reset(Region *region, RegionResetOption option);
Region *region_get_last_node(Region *region, RegionError *error, RegionLocation location);

#define REGION_GET_CURRENT_FILE_LOCATION (RegionLocation){.file_name = __FILE__, .line = __LINE__, .func_name = __func__}

#define region_alloc(capacity, error) __region_alloc((capacity), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define region_push(region, size, error) __region_push((region), (size), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define region_shrink_capacity(region, option, error) __region_shrink_capacity((region), (option), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define region_collect(region, collected_size, error) __region_collect((region), (collected_size), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define region_clone(region, error) __region_clone((region), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define region_merge(region, option, error) __region_merge((region), (option), (error), (REGION_GET_CURRENT_FILE_LOCATION))

#define stack_region_alloc(capacity, error) __stack_region_alloc((capacity), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define stack_region_push(stack, size, error) __stack_region_push((stack), (size), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define stack_region_peek(stack, error) __stack_region_peek((stack), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define stack_region_pop(stack, error) __stack_region_pop((stack), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define stack_region_reset(stack, option) region_reset((Region *)(stack), (option))

#define stack_region_get_ref_count(stack) (size_t *)((stack)->data - sizeof(size_t))
#define stack_region_get_count(stack) *(stack_region_get_ref_count(stack))

// ----- * -----

#ifdef REGION_IMPLEMENTATION

Region *__region_alloc(size_t capacity, RegionError *error, RegionLocation location)
{
    if (capacity == 0) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY, location);
        return NULL;
    }

    if (capacity > REGION_SIZE_MAX - sizeof(Region)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY, location);
        return NULL;
    }

    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION, location);
        return NULL;
    }

    region->data = (char *)REGION_MALLOC(sizeof(char) * capacity);
    
    if (!region->data) {
        REGION_FREE(region);
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, location);
        return NULL;
    }

    region->capacity = capacity;
    region->size = 0;
    region->next = NULL;

    REGION_MEMCPY(region->data, "\0", region->capacity);

    return region;
}

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

void *__region_push(Region *region, size_t size, RegionError *error, RegionLocation location)
{
    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_PUSH_NO_REGION, location);
        return NULL;
    }

    if (size == 0) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_PUSH_SMALL_SIZE, location);
        return NULL;
    }

    if (size > REGION_SIZE_MAX - sizeof(Region)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_PUSH_LARGE_SIZE, location);
        return NULL;
    }

    Region *current = region;

    while (current->size + size > current->capacity) {
        if (!current->next) {
            current->next = region_alloc(current->capacity * 2 + size, NULL);
            if (!current->next) {
                REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION, location);
                return NULL;
            }
            current = current->next;
            break;
        }

        current = current->next;
    }

    void *result = current->data + current->size;

    current->size += size;

    return result;
}

void region_reset(Region *region, RegionResetOption option)
{
    if (option == REGION_RESET_OPTION_HARD) {
        Region *second = region->next;
        region_free(&second);
    }

    for (Region *i = region; i; i = i->next) {
        i->size = 0;
    }
}

void __region_shrink_capacity_helper(Region *region, RegionShrinkCapacityOption option, RegionError *error, RegionLocation location)
{
    if (region->size == region->capacity || region->size == 0) return;

    char *shrinked_buffer = (char *)REGION_MALLOC(sizeof(char) * region->size);

    if (!shrinked_buffer) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_SHRINK_CAPACITY_MALLOC, location);
        return;
    }

    REGION_MEMCPY(shrinked_buffer, region->data, region->size);
    REGION_FREE(region->data);

    region->capacity = region->size;
    region->data = shrinked_buffer;

    return;
}

void __region_shrink_capacity(Region *region, RegionShrinkCapacityOption option, RegionError *error, RegionLocation location)
{
    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_SHRINK_CAPACITY_NO_REGION, location);
        return;
    }

    if (option == REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT) {
        __region_shrink_capacity_helper(region, option, error, location);
        return;
    }

    for (Region *t = region; t; t = t->next) {
        __region_shrink_capacity_helper(t, option, error, location);
            
        if (error->code != REGION_ERROR_CODE_NO_ERROR) return;
    }
}

Region **__region_collect(Region *region, size_t *collected_size, RegionError *error, RegionLocation location)
{
    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_REGION, location);
        return NULL;
    }

    if (!collected_size) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_COLLECT_NO_COLLECTED_SIZE, location);
        return NULL;
    }

    size_t nodes_count = 0;

    for (Region *t = region; t; t = t->next) {
        nodes_count++;
    }

    Region **collection = (Region **)REGION_MALLOC(sizeof(Region *) * nodes_count);

    if (!collection) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_COLLECT_MALLOC_COLLECTION, location);
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

Region *__region_clone(Region *region, RegionError *error, RegionLocation location)
{
    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_CLONE_NO_REGION, location);
        return NULL;
    }

    Region *clone = region_alloc(region->capacity, NULL);

    if (!clone) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_ROOT, location);
        return NULL;
    }

    clone->capacity = region->capacity;
    clone->size = region->size;

    REGION_MEMCPY(clone->data, region->data, region->size);

    Region *clone_i = clone;

    for (Region *t = region->next; t; t = t->next) {
        Region *node = region_alloc(t->capacity, NULL);

        if (!node) {
            region_free(&clone);
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_CLONE_MALLOC_NODE, location);
            return NULL;
        }

        node->capacity = t->capacity;
        node->size = t->size;

        REGION_MEMCPY(node->data, t->data, t->size);

        clone_i->next = node;
        clone_i = clone_i->next;
    }

    return clone;
}

Region *__region_merge(Region *region, RegionMergeOption option, RegionError *error, RegionLocation location)
{
    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_MERGE_NO_REGION, location);
        return NULL;
    }

    size_t collected_size = 0;
    Region **collection = region_collect(region, &collected_size, NULL);

    if (!collection) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_COLLECTION, location);
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

    Region *merged_region = region_alloc(merged_capacity, NULL);

    if (!merged_region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_MERGE_MALLOC_REGION, location);
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

Region *region_get_last_node(Region *region, RegionError *error, RegionLocation location)
{
    if (!region) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_GET_LAST_NODE_NO_REGION, location);
        return NULL;
    }

    Region *last_node = region;

    while (last_node->next != NULL) last_node = last_node->next;

    return last_node;
}

StackRegion *__stack_region_alloc(size_t capacity, RegionError *error, RegionLocation location)
{
    if (capacity == 0) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_SMALL_CAPACITY, location);
        return NULL;
    }

    if (capacity > REGION_SIZE_MAX - sizeof(StackRegion)) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_ALLOC_LARGE_CAPACITY, location);
        return NULL;
    }
    
    Region *stack = __region_alloc(capacity + sizeof(size_t), error, location);
    
    if (error->code == REGION_ERROR_CODE_NO_ERROR) {
        size_t *count = (size_t *)stack->data;
        *count = 0;
        stack->data += sizeof(size_t);
        return (StackRegion *)stack;
    }

    switch (error->code) {
        case REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION:
            error->code = REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_REGION; break;

        case REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY:
            error->code = REGION_ERROR_CODE_ENOMEM_STACK_REGION_ALLOC_MALLOC_CAPACITY; break;
        
        default: break;
    }
    
    return NULL;
}

void *__stack_region_push(StackRegion *stack, size_t size, RegionError *error, RegionLocation location)
{
    if (size == 0) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_SMALL_SIZE, location);
        return NULL;
    }
    if (size + sizeof(size_t) < size) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_LARGE_SIZE, location);
        return NULL;
    }

    void *frame = __region_push((Region *)stack, size + sizeof(size_t), error, location);

    if (error->code == REGION_ERROR_CODE_NO_ERROR) {
        *stack_region_get_ref_count(stack) += 1;
        *(size_t *)(frame + size) = size;
        return frame;
    }

    switch (error->code) {
        case REGION_ERROR_CODE_EINVAL_REGION_PUSH_NO_REGION:
            error->code = REGION_ERROR_CODE_EINVAL_STACK_REGION_PUSH_NO_STACK_REGION; break;

        case REGION_ERROR_CODE_ENOMEM_REGION_PUSH_MALLOC_REGION:
            error->code = REGION_ERROR_CODE_ENOMEM_STACK_REGION_PUSH_MALLOC_REGION; break;

        default: break;
    }

    return NULL;
}

void *__stack_region_peek(StackRegion *stack, RegionError *error, RegionLocation location)
{
    if (!stack) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_PEEK_NO_STACK_REGION, location);
        return NULL;
    }

    if (stack_region_get_count(stack) == 0) return NULL;

    Region *last_node = (Region *)stack;

    while (last_node->next != NULL && (last_node->next)->size > 0) {
        last_node = last_node->next;
    }

    void *last_frame_end = last_node->data + last_node->size;

    size_t last_frame_size = *(size_t *)(last_frame_end - sizeof(size_t));

    void *last_frame_start = last_frame_end - sizeof(size_t) - last_frame_size;

    return last_frame_start;
}

void *__stack_region_pop(StackRegion *stack, RegionError *error, RegionLocation location)
{
    if (!stack) {
        REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_STACK_REGION_POP_NO_STACK_REGION, location);
        return NULL;
    }

    if (stack_region_get_count(stack) == 0) return NULL;

    Region *last_node = (Region *)stack;

    while (last_node->next != NULL && last_node->size > 0) {
        last_node = last_node->next;
    }

    void *last_frame_end = last_node->data + last_node->size;

    size_t last_frame_size = *(size_t *)(last_frame_end - sizeof(size_t));

    void *last_frame_start = last_frame_end - sizeof(size_t) - last_frame_size;

    *stack_region_get_ref_count(stack) += 1;
    (last_node)->size -= (last_frame_size + sizeof(size_t));

    return last_frame_start;
}

void stack_region_free(StackRegion **stack)
{
    if (!stack || !(*stack)) return;

    (*stack)->data -= sizeof(size_t);

    region_free((Region **)stack);
}

#endif // REGION_IMPLEMENTATION
REGION_EXTERN_C_END