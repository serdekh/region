#pragma once

#include <stdbool.h>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

void *rt_so_open(const char *file_path);
void *rt_so_get_symbol(void *handle, const char *symbol_name);
char *rt_so_get_error();
void  rt_so_close(void *handle);