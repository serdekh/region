#pragma once 

#include "../../../src/region.h"
#include "../include/shared.h"

#include <dirent.h>
#include <errno.h>
#include <string.h>

#define _GNU_SOURCE

void *try_load_shared_object(const char *file_path);

bool try_load_file_and_test(const char *file_path);

void try_load_files_and_test(const char *directory);