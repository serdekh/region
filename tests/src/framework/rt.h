#pragma once 

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

#include "../include/rt-shared.h"

#define _GNU_SOURCE

void *rt_try_load_shared_object(const char *file_path);

bool rt_try_load_file_and_test(RegionAPI *api, const char *file_path);

bool rt_try_load_files_and_test(const char *directory);