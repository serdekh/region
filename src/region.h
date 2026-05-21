/**
 * @file region.h
 * @author Serhii Dekhtiarov (@SerhiiDekhtiarov - telegram)
 * @brief The source code for the region library
 * 
 *     C library for managing dynamic memory using the 
 *     region (aka arena) data structure.
 * 
 *     This library is written in the stb-style meaning 
 *     both declarations and implementations are defined
 *     in a single file. The implementation is guarded by the 
 *     REGION_IMPLEMENTATION macro. It only has to be defined once,
 *     usually in the main file of a project. To get more 
 *     information about stb-style libraries, go to the `see` 
 *     section.
 * 
 * @see 
 *    stb single-file public domain libraries: https://github.com/nothings/stb
 *    stb libraries explained: https://youtu.be/kS_GqDp6IT4?si=gxoRAqRc_B2nmfEb
 * 
 * @date 5/17/2026
 * @copyright Copyright (c) 2026
 * 
 */

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
    /**
     * @brief Applies OS-specific attributes for the library functions
     * 
     * @details 
     *     By default all symbols on Windows are not exported
     *     and therefore the functions cannot be called. This attribute
     *     solves this issue by exporting the function api's. 
     */
    #define REGION_API __declspec(dllexport)
#else
    /**
     * @brief Applies OS-specific attributes for the library functions
     * 
     * @details 
     *     Linux symbols are exported by default but not on Windows.
     *     In case of Linux, this macro provides a function with the 
     *     default visibility attribute making function api's accessible
     */
    #define REGION_API __attribute__((visibility("default")))
#endif

/**
 * @brief Prevents symbol lookup errors when using C++
 * 
 * @details Specifies that the library functions should 
 * use the C-language calling convention when the project
 * is being compilied for C++. This way, no mangling is 
 * applied and the symbols are exported as they are defined
 */
#ifdef __cplusplus
  #define REGION_EXTERN_C_BEGIN extern "C" {
  #define REGION_EXTERN_C_END }
#else
  #define REGION_EXTERN_C_BEGIN
  #define REGION_EXTERN_C_END
#endif

/**
 * @brief The main struct for storing and manipulating dynamic data
 * 
 * @typedef Region
 * 
 * @details 
 *     The struct is defined as an opaque type. All the fields
 *     and logic has to be provided by the consumer that implements
 *     the definition. The default implementation defines `Region` as
 *     a linked list of character arrays where the data resides.
 */
typedef struct __Region Region;

/**
 * @brief Derivative of the `Region` data structure
 * 
 * @typedef StackRegion
 * 
 *    Represents a region that is capable of popping items
 *    from the top of the data section (LIFO order).
 * 
 *    Similar to the program stack, the data gets pushed to
 *    the data array but in this case it also includes the
 *    size of a data represented with the size_t data type. 
 *    In other words, when you push `capacity` bytes to the 
 *    stack, in reality `capacity + sizeof(size_t)` gets 
 *    pushed. The reason lies in necessity to pop items with
 *    an arbitrary size. Without additional size, it would not
 *    be possible to correctly identify the amount of bytes
 *    to pop when needed. 
 * 
 *    This data structure is recommended if additional memory
 *    overhead is not as significant as an ability of dynamic
 *    data array mutation.
 * 
 * @details 
 *     This type (in the standard implementation) is identical
 *     to the `Region` struct. The core difference lies in the
 *     logic that is associated with the type in its 
 *     corresponding functions such as `stack_region_alloc`.
 *     Another difference lies in data shadowing used to store
 *     metadata such as the stack `count`. This provides a unified 
 *     pattern and allows using the `Region` function calls in
 *     the inner implementations for the `StackRegion`. 
 *     Although manually casting between `Region` and 
 *     `StackRegion` is generally not recommended.
 */
typedef struct __StackRegion StackRegion;

/**
 * @typedef StackRegionFrame
 * @brief Data type for representing stack values
 */
typedef struct _StackRegionFrame {

    void *data; /** Pointer to the beginning of the frame */

    size_t size; /** Length of the `data` type member */

} StackRegionFrame;

/**
 * @brief 
 *     Collection of data to represent the exact location 
 *     where an error has occured.
 * 
 * @typedef RegionLocation
 * 
 * @note 
 *     This type is generally only used internally. 
 *     The `RegionError` type already contains a
 *     member of type `RegionLocation` that automatically
 *     gets initialized by the `REGION_ERROR_INIT` macro
 *     which is achieved using the C preprocessor and gcc
 *     macros. 
 */
typedef struct {

    /** File line where an error has occured*/
    int line;

    /** File name where an error has occured*/
    const char *file_name;

    /** Function that has thrown an error */
    const char *func_name;

} RegionLocation;

/**
 * @brief Represents the general category an error falls into
 * 
 * @typedef RegionErrorType
 * 
 * @details
 *     The error system splits an error message into 4 main categories:
 *     '`type`: `function class`_`function name`(): `message`'. This
 *     enum holds options for the `type` category. 
 */
typedef enum {
    REGION_ERROR_TYPE_NONE,             /** No error has occurred */
    REGION_ERROR_TYPE_INVALID_ARGUMENT, /** Function argument is `NULL` or has invalid value*/
    REGION_ERROR_TYPE_NO_MEMORY,        /** Failed to allocate memory */
    REGION_ERROR_TYPE_UNKNOWN,          /** If none of the above matches the error type*/
} RegionErrorType;

/**
 * @brief Represents a class of functions depending on a type they work with
 * 
 * @typedef RegionErrorClass
 * 
 * @details
 *     The error system splits an error message into 4 main categories:
 *     '`type`: `function class`_`function name`(): `message`'. This
 *     enum holds options for the `function class` category. 
 */
typedef enum {
    REGION_ERROR_CLASS_REGION,      /** Class for all the `region_<name>` functions*/
    REGION_ERROR_CLASS_STACK_REGION /** Class for all the `stack_region_<name>` functions*/
} RegionErrorClass;

/**
 * @brief Represents a class-agnostic function name that has occured an error 
 * 
 * @typedef RegionErrorFunction
 * 
 * @details
 *     The error system splits an error message into 4 main categories:
 *     '`type`: `function class`_`function name`(): `message`'. This
 *     enum holds options for the `function name` category. 
 */
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

/**
 * @brief Provides opcodes which represent an error message
 * 
 * @typedef RegionErrorMessage
 * 
 * @details
 *     The error system splits an error message into 4 main categories:
 *     '`type`: `function class`_`function name`(): `message`'. This
 *     enum holds options for the `message` category. 
 */
typedef enum {
    REGION_ERROR_MESSAGE_ARG_LARGE_CAPACITY,              /** The value of `capacity` is equal to `SIZE_MAX`       */
    REGION_ERROR_MESSAGE_ARG_LARGE_SIZE,                  /** The value of `size` is equal to `SIZE_MAX`           */
    REGION_ERROR_MESSAGE_ARG_SMALL_SIZE,                  /** The value of `size` is equal to `0`                  */
    REGION_ERROR_MESSAGE_ARG_NULLPTR,                     /** The value of one of the arguments is equal to `NULL` */

    REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION,           /** Failed to allocate the `Region` type                 */
    REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_DATA,      /** Failed to allocate data for the `Region` type        */

    REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_ARRAY,     /** Failed to allocate an array of `Region` types        */
    REGION_ERROR_MESSAGE_MALLOC_FAILURE_TEMPORARY_BUFFER, /** Failed to allocate a temporary chunk of memory       */

    REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA      /** The `StackRegion` type's data is in invalid state    */
} RegionErrorMessage;

/**
 * @brief Represents an error that may occur during a function's execution
 * 
 * @typedef RegionError
 * 
 *     This type is used in all the functions that may fail and is treated as
 *     optional which lets a caller to provide a `NULL` pointer if the error
 *     is guaranteed not to happen or is not important. 
 * 
 *     The type also contains a member of type `RegionLocation`. This subtype
 *     is used for a proper error logging. A caller needs to init its
 *     values before a function call. This is done via the `REGION_ERROR_INIT`
 *     macro. See code examples.
 * 
 * @code{.c}
 * 
 *     RegionError error = REGION_ERROR_INIT;
 * 
 *     Region *region - region_alloc(SIZE_MAX, &error);
 * 
 *     if (error.type == REGION_ERROR_TYPE_NONE) {
 *         region_free(&region);
 *         return 0;
 *     }
 * 
 *     region_error_print(error);
 *     return 1;
 *     
 * @endcode
 */
typedef struct {
    RegionLocation location;

    REGION_UINT8 type;
    REGION_UINT8 function_class;
    REGION_UINT8 function;
    REGION_UINT8 message;
} RegionError;

REGION_EXTERN_C_BEGIN

/**
 * @brief Returns a newly allocated region.
 *
 * @param[in]      capacity Number of bytes for region data.
 * @param[in, out] error Optional error output.
 * 
 * @return 
 *     Pointer to a newly allocated `Region` struct. 
 *     Ownership is passed to the caller.
 *     Must be freed with the `region_free` function.
 * 
 * @warning Memory leak if the returned pointer is not freed with the `region_free` function.
 * 
 * @note 
 *     Region fields are initialized to
 *     - `0`    for `numeric` types.
 *     - `NULL` for pointers.
 * 
 * @note 
 *     The value of `capacity` can have a value of `0`. In this case, a region
 *     is still being allocated but its `data` is going to be empty. Other 
 *     functions will try to allocate the memory if they need it. For example,
 *     the `region_push(... size, ...)` function accepts a `size` argument and
 *     a region to allocate into. If the `data` is `NULL`, it'll try to 
 *     allocate that memory instead. 
 *
 *     This is usually not the best option since other functions only allocate
 *     the requested size and therefore additional nodes would need to be 
 *     created to push more items. 
 *
 * @retval 
 *    If any errors occur, the `error` argument will be initialized as follows:
 *    - `type`           = REGION_ERROR_TYPE_INVALID_ARGUMENT (If capacity is too large)
 *                         REGION_ERROR_TYPE_NO_MEMORY        (If failed to allocate a region or its data)
 * 
 *    - `function_class` = REGION_ERROR_CLASS_REGION
 * 
 *    - `function`       = REGION_ERROR_FUNCTION_ALLOC
 * 
 *    - `message`        = REGION_ERROR_MESSAGE_ARG_LARGE_CAPACITY         (If capacity is too large)
 *                         REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION      (If failed to allocate a region)
 *                         REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_DATA (If failed to allocate data for the region)
 * 
 * @code{.c}
 *     size_t capacity = 4 * sizeof(int);
 *
 *     RegionError error = REGION_ERROR_INIT;
 *
 *     Region *region = region_alloc(capacity, &error);
 *     
 *     if (error.type != REGION_ERROR_TYPE_NONE) {
 *         region_error_print(error);
 *         return 1;
 *     }
 *     
 *     region_free(&region);
 *     return 0;
 * @endcode
 */
REGION_API Region *region_alloc(size_t capacity, RegionError *error);

/**
 * @brief 
 *     Frees the `Region` struct previously 
 *     allocated via the `region_alloc` function.
 * 
 * @param[in, out] region 
 *     The pointer to the `Region` struct to deallocate.
 *     If `region` is equal to `NULL` the function 
 *     perfoms an early return and does nothing.
 * 
 * @details
 *     Alongside the `Region` struct, its corresponding
 *     data also gets deallocated and the `region` 
 *     argument is set to point to the `NULL` reference.
 * 
 * @code{.c}
 * 
 * #define N 1
 * 
 *     RegionError error = REGION_ERROR_INIT;
 * 
 *     Region *region = region_alloc(N, &error);
 * 
 *     if (error.type != REGION_ERROR_TYPE_NONE) {
 *         region_error_print(error);
 *         return 1;
 *     }
 * 
 *     region_free(&region);
 *     return 0;
 * 
 * @endcode
 */
REGION_API void region_free(Region **region);

/**
 * @brief Resets the data inside the `Region` struct.
 * 
 * @param[in, out] region 
 *     The target region that will be reset.
 *     If the `region` argument is equal to
 *     `NULL` the function performs an early
 *     return and does nothing.
 * 
 * @param[in] option 
 *     Additional parameters on how to reset the region. Here are
 *     the values that the function expects to work with:
 * 
 *     - REGION_RESET_OPTION_SOFT 
 * 
 *         Goes through each node of the region and sets
 *         their `size` values back to `0`.
 * 
 *     - REGION_RESET_OPTION_HARD
 * 
 *         Frees all the nodes except the first one. 
 *         The value of `size` in the root node is
 *         set to `0`.
 * 
 * @details
 *    Every instance of the `Region` struct holds a reference to
 *    the data it works with. When a chunk of data gets allocated
 *    the value of the `size` field gets increased by that amount.
 *    This function performs setting the value of `size` back to
 *    zero. Physically the data is still in memory but it is now
 *    accessible by any other consumers. 
 * 
 * @code{.c}
 * 
 *#define unwrap if (error.type == REGION_ERROR_TYPE_NONE) goto error
 * 
 *    RegionError error = REGION_ERROR_INIT;
 * 
 *    Region *region = region_alloc(sizeof(int), &error); unwrap;
 * 
 *    int *pushed = region_push_int(&region, &error); unwrap;
 * 
 *    printf("Value: %d, Size: %zu\n",
 *        pushed, region_get_size(region));
 * 
 *    region_reset(region, REGION_RESET_OPTION_SOFT);
 * 
 *    printf("Value: %d, Size: %zu\n",
 *        pushed, region_get_size(region));
 * 
 *    printf("The value of `size` have changed but the data remained the same\n");
 *    return 0;
 * 
 *error:
 *    region_error_print(error);
 *     return 1;
 * 
 * @endcode
 */
REGION_API void region_reset(Region *region, int option);

/**
 * @brief Returns the amount of bytes allocated for the `region` `data`.
 * 
 * @details
 *     The standard implementation of the `Region`
 *     data type contains a member called `capacity`.
 *     It stores the amount of bytes that are 
 *     allocated for the `data` member. `capacity`
 *     is always bigger or equal to the `size` member.
 * 
 * @param[in] region 
 *     The target to get the capacity from.
 *     If `region` points to `NULL`, the 
 *     value of `0` is returned.
 * 
 * @return the length of the `data` array
 * 
 * @code{.c}
 * 
 *#define N 5
 *
 *    RegionError error = REGION_ERROR_INIT;
 * 
 *    Region *region = region_alloc(N, &error);
 * 
 *    if (error.type != REGION_ERROR_TYPE_NONE) {
 *        region_error_print(error);
 *        return 1;
 *    }
 * 
 *    size_t capacity = region_get_capacity(region);
 * 
 *    printf("Actual: %zu, Expected: %zu\n", capacity, N);
 * 
 *    region_free(&region);
 * 
 *    return 0;
 * 
 * @endcode
 */
REGION_API size_t region_get_capacity(Region *region);

/**
 * @brief Returns the amount of bytes that are taken in the `region` `data`.
 * 
 * @details
 *     When a `Region` data type gets allocated with a 
 *     certain capacity, initially, the `data` member
 *     is initialized with the `0` value. When an
 *     allocation inside the region occurs, the value
 *     of `size` gets increased by that same amount.
 *     Its value is always in this range: `[0; capacity]`.
 * 
 * @param region 
 *     The target to get the size from.
 *     If the `region` points to `NULL`,
 *     The value of `0` is returned.
 * 
 * @return The amont of the taken bytes in the `data` array
 * 
 * @code{.c}
 * 
 *#define N 6
 *#define N_TAKEN N / 2
 *
 *#define unwrap if (error.type != REGION_ERROR_TYPE_NONE) goto error
 *
 *    RegionError error = REGION_ERROR_INIT;
 * 
 *    Region *region = region_alloc(N, &error); unwrap;
 * 
 *    region_push(&region, N_TAKEN, &error); unwrap;
 * 
 *    size_t size = region_get_size(region);
 *    size_t capacity = region_get_capacity(region);
 * 
 *    printf("Actual size: %zu, Expected size: %zu, Capacity: %zu\n",
 *        size, N_TAKEN, capacity);
 * 
 *    region_free(&region);
 * 
 *    return 0;
 * 
 * error:
 *     region_free(&region);
 *     region_error_print(error);
 *     return 1;
 * 
 * @endcode  
 */
REGION_API size_t region_get_size(Region *region);

/**
 * @brief 
 *     Allocates a copy of the `region` including
 *     all the following nodes.
 * 
 * @details
 *     The function iterates through the entire
 *     list and performs an alloction for each
 *     node. If at any point a node allocation
 *     fails, then the entire clone gets freed.
 *     The `data` in each cloned node is also
 *     copied and filled with the original 
 *     `data`. Other type members are also
 *     initialized with the original node value.
 * 
 * @warning 
 *     The returned value has to be freed in
 *     order to avoid a memory leak
 * 
 * @param[in] region
 *     The target region to read and allocate
 *     a copy from. If it points to `NULL`, 
 *     the function returns early with the
 *     value of `NULL`.
 * 
 * @param[in, out] error
 *     The optional error output. 
 * 
 * @retval 
 *    In case of any errors, the `error` argument
 *    will be initialized. The standard implementation
 *    relies on the `region_alloc` function which is
 *    used internally inside the `region_clone` function.
 *    Therefore, all the values for the `error` type
 *    members are identical to the `region_alloc` failure
 *    cases, except the `RegionError.function` member. 
 *    It's set to `REGION_ERROR_FUNCTION_CLONE`.
 * 
 * @return 
 *     A pointer to the beginning of a newly 
 *     allocated region. Ownership is passed
 *     to the caller. Must be freed using the
 *     `region_free` function.
 */
REGION_API Region *region_clone(Region *region, RegionError *error);
REGION_API Region *region_merge(Region *region, int option, RegionError *error);
REGION_API Region *region_get_last_node(Region *region, int option);
REGION_API Region **region_collect(Region *region, size_t *collected_size, RegionError *error);

REGION_API void *region_push(Region **region, size_t size, RegionError *error);
REGION_API int *region_push_int(Region **region, int value, RegionError *error);
REGION_API float *region_push_float(Region **region, float value, RegionError *error);
REGION_API double *region_push_double(Region **region, double value, RegionError *error);
REGION_API char *region_push_char(Region **region, char value, RegionError *error);

REGION_API void region_shrink_capacity(Region *region, int option, RegionError *error);

REGION_API StackRegion *stack_region_alloc(size_t capacity, RegionError *error);
REGION_API void stack_region_free(StackRegion **stack);

REGION_API size_t stack_region_get_capacity(StackRegion *region);
REGION_API size_t stack_region_get_size(StackRegion *region);
REGION_API size_t stack_region_get_count(StackRegion *region);

REGION_API StackRegionFrame stack_region_push(StackRegion **stack, size_t size, RegionError *error);
REGION_API int *stack_region_push_int(StackRegion **stack, int value, RegionError *error);
REGION_API float *stack_region_push_float(StackRegion **stack, float value, RegionError *error);
REGION_API double *stack_region_push_double(StackRegion **stack, double value, RegionError *error);
REGION_API char *stack_region_push_char(StackRegion **stack, char value, RegionError *error);

REGION_API StackRegionFrame stack_region_pop(StackRegion *stack, RegionError *error);
REGION_API int *stack_region_pop_int(StackRegion *stack, RegionError *error);
REGION_API float *stack_region_pop_float(StackRegion *stack, RegionError *error);
REGION_API double *stack_region_pop_double(StackRegion *stack, RegionError *error);
REGION_API char *stack_region_pop_char(StackRegion *stack, RegionError *error);

REGION_API StackRegionFrame stack_region_peek(StackRegion *stack, RegionError *error);
REGION_API StackRegionFrame stack_region_peek_at(StackRegion *stack, size_t index, RegionError *error);

REGION_API void stack_region_swap(StackRegion *stack, RegionError *error);

REGION_API void region_error_print_to(REGION_FILE *stream, RegionError error);
REGION_API void region_error_print(RegionError error);

#define REGION_RESET_OPTION_SOFT                0  /** Resets the occupated size in each node to zero              */
#define REGION_RESET_OPTION_HARD                1  /** Frees all the nodes expect the 1st and puts resets its size */

#define REGION_MERGE_OPTION_DEFAULT             0  /** All the data gets merged disregarding the the taken size    */
#define REGION_MERGE_OPTION_CONDENSE            1  /** Only the taken part of a region data gets merged            */

#define REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT 0  /** Only the root node gets its capacity shrinked               */
#define REGION_SHRINK_CAPACITY_OPTION_ALL       1  /** All subsequent nodes get their capacity shrinked            */

#define REGION_GET_LAST_NODE_OPTION_DEFAULT     0  /** Get the last node (next points to `NULL`)                   */
#define REGION_GET_LAST_NODE_OPTION_NON_EMPTY   1  /** Get the last node whose data has been taken                 */

/** Returns `true` if `capacity` and `size` members are equal to `0`. Returns `false` otherwise */
#define REGION_IS_EMPTY(region) region_get_capacity(region) == 0 && region_get_size(region) == 0;

/** Returns an initialized `RegionLocation` object for the current file, line and function */
#define REGION_GET_CURRENT_FILE_LOCATION (RegionLocation){.file_name = __FILE__, .line = __LINE__, .func_name = __func__}       

/** Returns a `RegionError` object (`location` is set to the current location, all other fields are set to `0`) */
#define REGION_ERROR_INIT (RegionError){.function = 0, .function_class = 0, .message = 0, .type = 0, .location = REGION_GET_CURRENT_FILE_LOCATION }

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

#define REGION_ERROR_SET(error, _type, _function_class, _function, _message) \
    if ((error)) {                                                       \
        (error)->type = _type;                                            \
        (error)->function_class = _function_class;                        \
        (error)->function = _function;                                    \
        (error)->message = _message;                                      \
    }                                                                    \

Region *region_alloc(size_t capacity, RegionError *error)
{
    if (capacity > REGION_SIZE_MAX - sizeof(Region)) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_REGION, 
            REGION_ERROR_FUNCTION_ALLOC, 
            REGION_ERROR_MESSAGE_ARG_LARGE_CAPACITY);
        return NULL;
    }

    Region *region = (Region *)REGION_MALLOC(sizeof(Region));

    if (!region) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_NO_MEMORY, 
            REGION_ERROR_CLASS_REGION, 
            REGION_ERROR_FUNCTION_ALLOC, 
            REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION);
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
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_NO_MEMORY, 
            REGION_ERROR_CLASS_REGION, 
            REGION_ERROR_FUNCTION_ALLOC, 
            REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_DATA);
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

// TODO: Add documentation similar to region_alloc
void *region_push(Region **region, size_t size, RegionError *error)
{
    if (!region) return NULL;

    if (size == 0) return NULL;

    if (size > REGION_SIZE_MAX - sizeof(Region)) {
        REGION_ERROR_SET(error,
            REGION_ERROR_TYPE_INVALID_ARGUMENT,
            REGION_ERROR_CLASS_REGION,
            REGION_ERROR_FUNCTION_PUSH,
            REGION_ERROR_MESSAGE_ARG_LARGE_SIZE);
        return NULL;
    }

    if (!(*region)) {
        *region = region_alloc(size, error);

        if (!(*region)) {
            if (error) error->function = REGION_ERROR_FUNCTION_PUSH;
            return NULL;
        }
    }

    Region *current = *region;

    if (!current->data) {
        current->data = (char *)REGION_MALLOC(size * sizeof(char));

        if (!current->data) {
            REGION_ERROR_SET(error,
            REGION_ERROR_TYPE_INVALID_ARGUMENT,
            REGION_ERROR_CLASS_REGION,
            REGION_ERROR_FUNCTION_PUSH,
            REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION);
            return NULL;
        }

        current->capacity = size * sizeof(char);
    }

    while (current->size + size > current->capacity) {
        if (current->next) {
            current = current->next;
            continue;
        }
        
        current->next = region_alloc(current->capacity * 2 + size, error);

        if (!current->next) {
            if (error) error->function = REGION_ERROR_FUNCTION_PUSH;
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

    int *result = (int *)region_push(region, sizeof(int), error);

    if (result) {
        *result = value;
        return result;
    }

    if (error) error->function = REGION_ERROR_FUNCTION_PUSH_INT;
    
    return NULL;
}

float *region_push_float(Region **region, float value, RegionError *error)
{
    if (!region) return NULL;

    float *result = (float *)region_push(region, sizeof(float), error);

    if (result) {
        *result = value;
        return result;
    }

    if (error) error->function = REGION_ERROR_FUNCTION_PUSH_FLOAT;

    return NULL;
}

double *region_push_double(Region **region, double value, RegionError *error)
{
    if (!region) return NULL;
    
    double *result = (double *)region_push(region, sizeof(double), error);
    
    if (result) {
        *result = value;
        return result;
    }

    if (error) error->function = REGION_ERROR_FUNCTION_PUSH_DOUBLE;
    
    return NULL;
}

char *region_push_char(Region **region, char value, RegionError *error)
{
    if (!region) return NULL;

    char *result = (char *)region_push(region, sizeof(char), error);

    if (result) {
        *result = value;
        return result;
    }

    if (error) error->function = REGION_ERROR_FUNCTION_PUSH_CHAR;

    return NULL;
}

void region_reset(Region *region, int option)
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
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_NO_MEMORY, 
            REGION_ERROR_CLASS_REGION,
            REGION_ERROR_FUNCTION_SHRINK_CAPACITY,
            REGION_ERROR_MESSAGE_MALLOC_FAILURE_TEMPORARY_BUFFER);
        return;
    }

    REGION_MEMCPY(shrinked_buffer, region->data, region->size);
    REGION_FREE(region->data);

    region->capacity = region->size;
    region->data = shrinked_buffer;
}

void region_shrink_capacity(Region *region, int option, RegionError *error)
{
    if (!region) return;

    RegionError local_error = REGION_ERROR_INIT;
    
    if (option == REGION_SHRINK_CAPACITY_OPTION_ONLY_ROOT) {
        __region_shrink_capacity_helper(region, error);
        return;
    }

    for (Region *t = region; t; t = t->next) {
        __region_shrink_capacity_helper(t, &local_error);
            
        if (local_error.type != REGION_ERROR_TYPE_NONE) {
            if (error) *error = local_error;
            return;
        }
    }
}

Region **region_collect(Region *region, size_t *collected_size, RegionError *error)
{
    if (!region) return NULL;

    if (!collected_size) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_REGION, 
            REGION_ERROR_FUNCTION_COLLECT, 
            REGION_ERROR_MESSAGE_ARG_NULLPTR);
        return NULL;
    }

    size_t nodes_count = 0;

    for (Region *t = region; t; t = t->next) {
        nodes_count++;
    }

    Region **collection = (Region **)REGION_MALLOC(sizeof(Region *) * nodes_count);

    if (!collection) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_REGION, 
            REGION_ERROR_FUNCTION_COLLECT, 
            REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_ARRAY);
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

    Region *clone = region_alloc(region->capacity, error);

    if (!clone) {
        if (error) error->function = REGION_ERROR_FUNCTION_CLONE;
        return NULL;
    }

    clone->capacity = region->capacity;
    clone->size = region->size;

    if (region->data) REGION_MEMCPY(clone->data, region->data, region->size);

    Region *clone_i = clone;

    for (Region *t = region->next; t; t = t->next) {
        Region *node = region_alloc(t->capacity, error);

        if (!node) {
            region_free(&clone);
            if (error) error->function = REGION_ERROR_FUNCTION_CLONE;
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

Region *region_merge(Region *region, int option, RegionError *error)
{
    if (!region) return NULL;

    size_t collected_size = 0;
    Region **collection = region_collect(region, &collected_size, error);

    if (!collection) {
        if (error) error->function = REGION_ERROR_FUNCTION_MERGE;
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

        Region *result = region_alloc(0, error);

        if (!result && error) error->function = REGION_ERROR_FUNCTION_MERGE;

        return result;
    }

    Region *merged_region = region_alloc(merged_capacity, error);

    if (!merged_region) {
        REGION_FREE(collection);
        if (error) error->function = REGION_ERROR_FUNCTION_MERGE;
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

Region *region_get_last_node(Region *region, int option)
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
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_ALLOC, 
            REGION_ERROR_MESSAGE_ARG_LARGE_CAPACITY);
        return NULL;
    }
    
    Region *stack = region_alloc(capacity + STACK_REGION_CACHE_COUNT_SIZE, error);
    
    if (!stack) {
        if (error) error->function_class = REGION_ERROR_CLASS_STACK_REGION;
        return NULL;
    }
    
    size_t *count = (size_t *)stack->data;
    *count = 0;
    stack->data += STACK_REGION_CACHE_COUNT_SIZE;

    return (StackRegion *)stack;
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
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_PUSH, 
            REGION_ERROR_MESSAGE_ARG_LARGE_SIZE);
        return STACK_REGION_FRAME_EMPTY;
    }

    RegionError local_error = {0};

    void *frame_data = region_push((Region **)stack, size + sizeof(size_t), &local_error);

    if (local_error.type == REGION_ERROR_TYPE_NONE) {
        *STACK_REGION_GET_COUNT_REF(*stack) += 1;
        *(size_t *)(frame_data + size) = size;

        StackRegionFrame frame = {0};
        frame.data = frame_data;
        frame.size = size;

        return frame;
    }

    if (error) {
        local_error.function_class = REGION_ERROR_CLASS_STACK_REGION;
        *error = local_error;
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

    if (local_error.type == REGION_ERROR_TYPE_NONE) {
        *(int *)result.data = value;
        return (int *)result.data;
    }

    if (error) {
        local_error.function = REGION_ERROR_FUNCTION_PUSH_INT;
        *error = local_error;
    }

    return NULL;
}

float *stack_region_push_float(StackRegion **stack, float value, RegionError *error)
{
    if (!stack) return NULL;

    RegionError local_error = REGION_ERROR_INIT;

    StackRegionFrame result = stack_region_push(stack, sizeof(float), &local_error);

    if (local_error.type == REGION_ERROR_TYPE_NONE) {
        *(float *)result.data = value;
        return (float *)result.data;
    }

    if (error) {
        local_error.function = REGION_ERROR_FUNCTION_PUSH_FLOAT;
        *error = local_error;
    }

    return NULL;
}

double *stack_region_push_double(StackRegion **stack, double value, RegionError *error)
{
    if (!stack)  return NULL;

    RegionError local_error = REGION_ERROR_INIT;

    StackRegionFrame result = stack_region_push(stack, sizeof(double), &local_error);

    if (local_error.type == REGION_ERROR_TYPE_NONE) {
        *(double *)result.data = value;
        return (double *)result.data;
    }

    if (error) {
        local_error.function = REGION_ERROR_FUNCTION_PUSH_DOUBLE;
        *error = local_error;
    }

    return NULL;
}

char *stack_region_push_char(StackRegion **stack, char value, RegionError *error)
{
    if (!stack) return NULL;

    RegionError local_error = REGION_ERROR_INIT;

    StackRegionFrame result = stack_region_push(stack, sizeof(char), &local_error);

    if (local_error.type == REGION_ERROR_TYPE_NONE) {
        *(char *)result.data = value;
        return (char *)result.data;
    }

    if (error) {
        local_error.function = REGION_ERROR_FUNCTION_PUSH_CHAR;
        *error = local_error;
    }

    return NULL;
}

StackRegionFrame stack_region_peek(StackRegion *stack, RegionError *error)
{
    if (!stack) return STACK_REGION_FRAME_EMPTY;

    if (stack_region_get_count(stack) == 0) return STACK_REGION_FRAME_EMPTY;

    Region *last_node = region_get_last_node((Region *)stack, REGION_GET_LAST_NODE_OPTION_NON_EMPTY);

    if (!last_node || !(last_node->data)) return STACK_REGION_FRAME_EMPTY;

    if (last_node->size <= STACK_REGION_CACHE_COUNT_SIZE) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_PEEK, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
        return STACK_REGION_FRAME_EMPTY;
    }

    if (last_node->size - STACK_REGION_CACHE_COUNT_SIZE * stack_region_get_count(stack) < stack_region_get_count(stack)) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_PEEK, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
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

    if (last_node->size <= STACK_REGION_CACHE_COUNT_SIZE) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_PEEK_AT, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
        return STACK_REGION_FRAME_EMPTY;
    }

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
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_POP, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
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
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_POP_INT, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
        return NULL;
    }

    StackRegionFrame frame = stack_region_pop(stack, NULL);

    return (int *)(frame.data);
}

float *stack_region_pop_float(StackRegion *stack, RegionError *error)
{
    if (!stack) return NULL;
    
    if (stack_region_peek(stack, NULL).size != sizeof(float)) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_POP_FLOAT, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
        return NULL;
    }

    return (float *)stack_region_pop(stack, NULL).data;
}

double *stack_region_pop_double(StackRegion *stack, RegionError *error)
{
    if (!stack) return NULL;

    if (stack_region_peek(stack, NULL).size != sizeof(double)) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_POP_DOUBLE, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
        return NULL;
    }

    return (double *)stack_region_pop(stack, NULL).data;
}

char *stack_region_pop_char(StackRegion *stack, RegionError *error)
{
    if (!stack) return NULL;

    if (stack_region_peek(stack, NULL).size != sizeof(char)) {
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_INVALID_ARGUMENT, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_POP_CHAR, 
            REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA);
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
        REGION_ERROR_SET(error, 
            REGION_ERROR_TYPE_NO_MEMORY, 
            REGION_ERROR_CLASS_STACK_REGION, 
            REGION_ERROR_FUNCTION_SWAP, 
            REGION_ERROR_MESSAGE_MALLOC_FAILURE_TEMPORARY_BUFFER);
        return;
    }

    REGION_MEMCPY(temporary_buffer, last.data, last.size);
    REGION_MEMCPY(temporary_buffer + last.size, prev.data, prev.size);

    StackRegionFrame repushed_last = stack_region_push(&stack, last.size, error);

    if (STACK_REGION_FRAME_IS_EMPTY(repushed_last)) {
        REGION_FREE(temporary_buffer);
        if (error) error->function = REGION_ERROR_FUNCTION_SWAP;
        return;
    }
    
    StackRegionFrame repushed_prev = stack_region_push(&stack, prev.size, error);

    if (STACK_REGION_FRAME_IS_EMPTY(repushed_prev)) {
        stack_region_pop(stack, NULL);
        REGION_FREE(temporary_buffer);
        if (error) error->function = REGION_ERROR_FUNCTION_SWAP;
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
    if (error.type == REGION_ERROR_TYPE_NONE) {
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
        case REGION_ERROR_MESSAGE_ARG_NULLPTR:
            REGION_SPRINTF(message_string, "%s.", "Null reference in an argument"); break;
        case REGION_ERROR_MESSAGE_CORRUPTED_STACK_REGION_DATA:
            REGION_SPRINTF(message_string, "%s.", "The stack frame's size or data has been corrupted or misinterpretted"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate a `Region` struct"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_ARRAY:
            REGION_SPRINTF(message_string, "%s.", "Failed to allocate a an array of `Region` structs"); break;
        case REGION_ERROR_MESSAGE_MALLOC_FAILURE_REGION_DATA:
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
