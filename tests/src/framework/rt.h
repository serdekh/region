#pragma once 

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "../include/rt-shared.h"

bool rt_try_load_file_and_test(RegionAPI *api, const char *file_path);

bool rt_try_load_files_and_test(const char *directory);
