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

typedef struct __StackRegion {
    Region *frames;         // where the actual data resides
    Region *frames_indexes; // the indexes where stack frames are located
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

typedef struct {
    int line;
    ErrorCode code;
    char file_name[256];
    char func_name[32];
    char message[256];
} RegionError;

REGION_EXTERN_C_BEGIN

// ----- FUNCTION DECLARATIONS (PRIVATE) -----
void __region_log_error(RegionError error, FILE *out);
void __region_set_error(RegionError *error, ErrorCode error_code, const char *filename, int line, const char *func);
Region *__region_alloc(size_t capacity, RegionError *error, const char *filename, int line, const char *func);
void *__region_alloc_item(Region *region, size_t size, RegionError *error, const char *filename, int line, const char *func);

// Stack Region
StackRegion *__stack_region_alloc(size_t capacity, RegionError *error, const char *filename, int line, const char *func);
void *__stack_region_push(StackRegion *stack, size_t size, RegionError *error, const char *filename, int line, const char *func);
void *__stack_region_pop(StackRegion *stack, RegionError *error, const char *filename, int line, const char *func);
void __stack_region_reset(StackRegion *stack, RegionResetOption option);
void __stack_region_free(StackRegion **stack);

// ----- PUBLIC API -----
void region_free(Region **region);
void region_reset(Region *region, RegionResetOption option);

#define region_alloc(capacity, error) __region_alloc((capacity), (error), __FILE__, __LINE__, __func__)
#define region_alloc_item(region, size, error) __region_alloc_item((region), (size), (error), __FILE__, __LINE__, __func__)
#define region_log_error(error) __region_log_error((error), REGION_STDOUT)
// ----- * -----

#ifdef REGION_IMPLEMENTATION

void __region_set_error(RegionError *error, ErrorCode error_code, const char *filename, int line, const char *func)
{
    error->line = line;
    error->code = error_code;
    REGION_STRNCPY(error->file_name, filename, REGION_STRLEN(filename));
    REGION_STRNCPY(error->func_name, func, REGION_STRLEN(func));
}

Region *__region_alloc(size_t capacity, RegionError *error, const char *filename, int line, const char *func)
{
    if (capacity == 0) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_INVALID_ARGUMENT, filename, line, func);
            REGION_SPRINTF(error->message, "The `region` cannot have `capacity` equal to zero.");
        }
        return NULL;
    }

    if (capacity > REGION_SIZE_MAX - sizeof(Region)) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
            REGION_SPRINTF(error->message, "The value of `capacity` is too large %zu", capacity);
        }
        return NULL;
    }

    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
        }
        return NULL;
    }

    region->data = (char *)REGION_MALLOC(sizeof(char) * capacity);
    
    if (!region->data) {
        REGION_FREE(region);
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
        }
        return NULL;
    }

    region->capacity = capacity;
    region->size = 0;
    region->next = NULL;

    for (size_t i = 0; i < region->capacity; i++) region->data[i] = '\0';

    return region;
}

void __region_log_error(RegionError error, FILE *out)
{
    if (error.code == REGION_ERROR_TYPE_NO_ERROR) return;

    REGION_FPRINTF(out, "[Region][ERROR](%s:%d:%s()): ",
        error.file_name,
        error.line,
        error.func_name);

    switch (error.code) {
        case REGION_ERROR_TYPE_INVALID_ARGUMENT:
            REGION_FPRINTF(out, "Invalid arguments.");
            break;
        case REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY:
            REGION_FPRINTF(out, "Not enough memory to allocate.");
            break;
        default:
            break;
    }

    if (REGION_STRLEN(error.message) == 0) {
        REGION_FPRINTF(out, "\n");
    }
    else {
        REGION_FPRINTF(out, " %s.\n", error.message);
    }
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

void *__region_alloc_item(Region *region, size_t size, RegionError *error, const char *filename, int line, const char *func)
{
    if (!region) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_INVALID_ARGUMENT, filename, line, func);
            REGION_SPRINTF(error->message, "The `region` holds a null reference");
        }
        return NULL;
    }

    if (size == 0) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_INVALID_ARGUMENT, filename, line, func);
            REGION_SPRINTF(error->message, "The value of `size` cannot be equal to zero");
        }
        return NULL;
    }

    if (size > REGION_SIZE_MAX - sizeof(Region)) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
            REGION_SPRINTF(error->message, "The value of `size` is too large %zu", size);
        }
        return NULL;
    }

    Region *current = region;

    while (current->size + size > current->capacity) {
        if (!current->next) {
            current->next = region_alloc(current->capacity * 2 + size, NULL);
            if (!current->next) {
                if (error) {
                    __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
                    REGION_SPRINTF(error->message, ".\nWarning: The current `region` is not freed. Use `region_free` to avoid a memory leak");
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

StackRegion *__stack_region_alloc(size_t capacity, RegionError *error, const char *filename, int line, const char *func)
{
    StackRegion *stack = (StackRegion *)REGION_MALLOC(sizeof(StackRegion));

    if (!stack) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_NOT_ENOUGH_MEMORY, filename, line, func);
            REGION_SPRINTF(error->message, "Cannot allocate memory for a stack region (%zub)", sizeof(StackRegion));
        }
        return NULL;
    }

    stack->frames = __region_alloc(capacity, error, filename, line, func);

    if (!stack->frames) {
        REGION_FREE(stack);
        return NULL;
    }

    stack->frames_indexes = __region_alloc(capacity, error, filename, line, func);

    if (!stack->frames_indexes) {
        REGION_FREE(stack->frames);
        REGION_FREE(stack);
        return NULL;
    }

    return stack;
}

void *__stack_region_push(StackRegion *stack, size_t size, RegionError *error, const char *filename, int line, const char *func)
{
    if (!stack) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_INVALID_ARGUMENT, filename, line, func);
            REGION_SPRINTF(error->message, "The `stack` pointer is equal to `NULL`");
        }
        return NULL;
    }

    size_t *new_frame_size = (size_t *)__region_alloc_item(stack->frames_indexes, sizeof(size_t), error, filename, line, func);

    if (!new_frame_size) return NULL;

    void *new_frame = __region_alloc_item(stack->frames, size, error, filename, line, func);
    
    if (!new_frame) {
        stack->frames_indexes->size -= sizeof(size_t);
        return NULL;
    }

    *new_frame_size = size;

    return new_frame;
}

void *__stack_region_pop(StackRegion *stack, RegionError *error, const char *filename, int line, const char *func)
{
    if (!stack) {
        if (error) {
            __region_set_error(error, REGION_ERROR_TYPE_INVALID_ARGUMENT, filename, line, func);
            REGION_SPRINTF(error->message, "The `stack` pointer is equal to `NULL`");
        }
        return NULL;
    }

    // pop the last frame's size
    size_t last_frame_size = stack->frames_indexes->data[stack->frames_indexes->size - sizeof(size_t)];
    stack->frames_indexes->size -= sizeof(size_t);

    // pop the last item
    stack->frames->size -= last_frame_size;

    return stack->frames->data + stack->frames->size;
}

void __stack_region_reset(StackRegion *stack, RegionResetOption option)
{
    if (!stack) return;

    region_reset((Region *)(stack->frames), option);
    region_reset(stack->frames_indexes, option);
}

void __stack_region_free(StackRegion **stack)
{
    if (!stack || !(*stack)) return;

    region_free(&(*stack)->frames);
    region_free(&(*stack)->frames_indexes);

    REGION_FREE(*stack);
}


#endif // REGION_IMPLEMENTATION
REGION_EXTERN_C_END