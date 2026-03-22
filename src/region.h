// ----- HEADERS -----
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
#endif // REGION_NO_STRING

#ifndef REGION_NO_STDLIB
#include <stdlib.h>
#define REGION_MALLOC malloc
#define REGION_REALLOC realloc
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

#ifdef __cplusplus
  #define REGION_EXTERN_C_BEGIN extern "C" {
  #define REGION_EXTERN_C_END }
#else
  #define REGION_EXTERN_C_BEGIN
  #define REGION_EXTERN_C_END
#endif

// ----- DATA STRUCTS -----

#define REGION_PRIVATE_CORE_FIELDS \
    size_t capacity;               \
    size_t size;                   \
    char *data;                    \
    struct __Region *next;         \

typedef struct __Region {
    REGION_PRIVATE_CORE_FIELDS
} Region;

typedef struct {
    size_t *items;
    size_t capacity;
    size_t len;
} Uints;

typedef struct __StackRegion {
    Region *frames;         // where the actual data resides
    Uints frames_sizes; // the indexes where stack frames are located
} StackRegion;

typedef enum {
    REGION_RESET_OPTION_SOFT = 0,
    REGION_RESET_OPTION_HARD = 1,
} RegionResetOption;

// ----- DATA STRUCTS FOR ERRORS -----

typedef char ErrorCode;

#define REGION_ERROR_TYPE_NO_ERROR          (ErrorCode)0
#define REGION_ERROR_TYPE_INVALID_ARGUMENT  (ErrorCode)1
#define REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY (ErrorCode)2

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

    // __region_alloc_item
    REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_NO_REGION,    // The pointer to the `Region` struct equals to `NULL`.
    REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_SMALL_SIZE,   // The `size` argument equals to zero.
    REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_LARGE_SIZE,   // The `size` argument equals to `__SIZE_MAX__`.
    REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_ITEM_MALLOC_REGION // Failed to allocate the `Region` struct for a new item.

} RegionErrorCode;

static const char *region_error_code_as_strings[] = {
    "No error",

    // __region_alloc
    "Invalid argument: The value of `capacity` cannot equal to zero.",
    "Invalid argument: The value of `capacity` is too large. Cannot allocate memory.",
    "No free space: Failed to allocate a `Region` struct.",
    "No free space: Failed to allocate `capacity` bytes into the `Region` struct.",

    // __region_alloc_item
    "Invalid argument: The value of `region` cannot equal to `NULL`.",
    "Invalid argument: The value of `size` cannot equal to zero.",
    "Invalid argument: The value of `size` is too large. Cannot allocate memory.",
    "No free space: Failed to allocate a `Region` struct for a new item.",
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

#define REGION_SET_ERROR(error, error_code, location)          \
    (error)->code = (error_code);                              \
    (error)->location.line = (location).line;                  \
    (error)->location.file_name = (location).file_name;        \
    (error)->location.func_name = (location).func_name;        \

#define REGION_LOG_ERROR_TO(error, out)                        \
    REGION_FPRINTF((out), "[Region][ERROR](\"%s:%d:%s\"): %s\n", \
        (error).location.file_name,                            \
        (error).location.line,                                 \
        (error).location.func_name,                            \
        region_error_code_as_strings[(error).code]);           \

#define REGION_LOG_ERROR(error) REGION_LOG_ERROR_TO((error), REGION_STDERR)
#define REGION_NO_ERROR(error) (error).code == REGION_ERROR_CODE_NO_ERROR
#define REGION_ERROR(error) !(REGION_NO_ERROR((error)))

REGION_EXTERN_C_BEGIN

// ----- FUNCTION DECLARATIONS (PRIVATE) -----

Region *__region_alloc(size_t capacity, RegionError *error, RegionLocation location);
void *__region_alloc_item(Region *region, size_t size, RegionError *error, RegionLocation location);

// Stack Region
StackRegion *__stack_region_alloc(size_t capacity, RegionError *error, RegionLocation location);
void *__stack_region_push(StackRegion *stack, size_t size, RegionError *error, RegionLocation location);
void *__stack_region_pop(StackRegion *stack, RegionError *error, RegionLocation location);
void stack_region_reset(StackRegion *stack, RegionResetOption option);
void stack_region_free(StackRegion **stack);

// ----- PUBLIC API -----
void region_free(Region **region);
void region_reset(Region *region, RegionResetOption option);

#define REGION_GET_CURRENT_FILE_LOCATION (RegionLocation){.file_name = __FILE__, .line = __LINE__, .func_name = __func__}

#define region_alloc(capacity, error) __region_alloc((capacity), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define region_alloc_item(region, size, error) __region_alloc_item((region), (size), (error), (REGION_GET_CURRENT_FILE_LOCATION))

#define stack_region_alloc(capacity, error) __stack_region_alloc((capacity), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define stack_region_push(stack, size, error) __stack_region_push((stack), (size), (error), (REGION_GET_CURRENT_FILE_LOCATION))
#define stack_region_pop(stack, error) __stack_region_pop((stack), (error), (REGION_GET_CURRENT_FILE_LOCATION))
// ----- * -----

#ifdef REGION_IMPLEMENTATION

Region *__region_alloc(size_t capacity, RegionError *error, RegionLocation location)
{
    if (capacity == 0) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_SMALL_CAPACITY, location);
        }
        return NULL;
    }

    if (capacity > REGION_SIZE_MAX - sizeof(Region)) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_LARGE_CAPACITY, location);
        }
        return NULL;
    }

    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_REGION, location);
        }
        return NULL;
    }

    region->data = (char *)REGION_MALLOC(sizeof(char) * capacity);
    
    if (!region->data) {
        REGION_FREE(region);
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_MALLOC_CAPACITY, location);
        }
        return NULL;
    }

    region->capacity = capacity;
    region->size = 0;
    region->next = NULL;

    for (size_t i = 0; i < region->capacity; i++) region->data[i] = '\0';

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

void *__region_alloc_item(Region *region, size_t size, RegionError *error, RegionLocation location)
{
    if (!region) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_NO_REGION, location);
        }
        return NULL;
    }

    if (size == 0) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_SMALL_SIZE, location);
        }
        return NULL;
    }

    if (size > REGION_SIZE_MAX - sizeof(Region)) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL_REGION_ALLOC_ITEM_LARGE_SIZE, location);
        }
        return NULL;
    }

    Region *current = region;

    while (current->size + size > current->capacity) {
        if (!current->next) {
            current->next = region_alloc(current->capacity * 2 + size, NULL);
            if (!current->next) {
                if (error) {
                    REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM_REGION_ALLOC_ITEM_MALLOC_REGION, location);
                }
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

StackRegion *__stack_region_alloc(size_t capacity, RegionError *error, RegionLocation location)
{
    if (capacity == 0) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL, location);
        }
        return NULL;
    }

    if (capacity > REGION_SIZE_MAX - sizeof(Region)) {
        if (error) {
             REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL, location);
        }
        return NULL;
    }

    StackRegion *stack = (StackRegion *)REGION_MALLOC(sizeof(StackRegion));

    if (!stack) {
        if (error) {
             REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM, location);
        }
        return NULL;
    }

    stack->frames = __region_alloc(capacity, error, location);

    if (!stack->frames) {
        REGION_FREE(stack);
        return NULL;
    }

    stack->frames_sizes.items = REGION_MALLOC(sizeof(size_t) * capacity);

    if (!stack->frames_sizes.items) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM, location);
        }
        REGION_FREE(stack->frames);
        REGION_FREE(stack);
        return NULL;
    }

    for (size_t i = 0; i < capacity; i++) stack->frames_sizes.items[i] = 0;

    stack->frames_sizes.len = 0;

    return stack;
}

void *__stack_region_push(StackRegion *stack, size_t size, RegionError *error, RegionLocation location)
{
    if (!stack) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL, location);
        }
        return NULL;
    }

    if (stack->frames_sizes.len >= stack->frames_sizes.capacity) {
        size_t new_capacity = (stack->frames_sizes.capacity == 0) ? 256 : stack->frames_sizes.capacity * 2;

        size_t *new_items = REGION_REALLOC(stack->frames_sizes.items, new_capacity * sizeof(size_t));
        
        if (!new_items) {
            if (error) {
                REGION_SET_ERROR(error, REGION_ERROR_CODE_ENOMEM, location);
            }
            return NULL;
        }

        stack->frames_sizes.items = new_items;
        stack->frames_sizes.capacity = new_capacity;
    }

    void *frame = __region_alloc_item(stack->frames, size, error, location);

    if (error->code != 0) return NULL;

    stack->frames_sizes.items[stack->frames_sizes.len++] = size;
    
    return frame;
}

void *__stack_region_pop(StackRegion *stack, RegionError *error, RegionLocation location)
{
    if (!stack) {
        if (error) {
            REGION_SET_ERROR(error, REGION_ERROR_CODE_EINVAL, location);
        }
        return NULL;
    }

    if (stack->frames_sizes.len == 0 || !stack->frames) return NULL;

    size_t last_frame_size = stack->frames_sizes.items[stack->frames_sizes.len - 1];

    Region *last = stack->frames;

    while (last) {
        if (!last->next) break;
        last = last->next;
    }

    last->size -= last_frame_size;

    return (void*)(last->data + last->size);
}

void stack_region_reset(StackRegion *stack, RegionResetOption option)
{
    if (!stack) return;

    region_reset((Region *)(stack->frames), option);
    stack->frames_sizes.len = 0;
}

void stack_region_free(StackRegion **stack)
{
    if (!stack || !(*stack)) return;

    region_free(&((*stack)->frames));
    REGION_FREE((*stack)->frames_sizes.items);
    REGION_FREE(*stack);
}


#endif // REGION_IMPLEMENTATION
REGION_EXTERN_C_END