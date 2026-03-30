#include "./tester.h"

void *try_load_shared_object(const char *file_path)
{
    void *handle = dlopen(file_path, RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "[Test][Error]: Could not load a dynamic executable (%s): %s\nStop.\n",
            file_path, dlerror());
    }

    return handle;
}

bool try_load_file_and_test(const char *file_path)
{
    void *handle = try_load_shared_object(file_path);

    if (!handle) return false;

    const TestContext *(*get_start)(void) = dlsym(handle, GET_TESTS_START_STR);
    const TestContext *(*get_end  )(void) = dlsym(handle, GET_TESTS_END_STR);

    for (const TestContext *t = get_start(); t < get_end(); t++) {
        TestResult result = t->func();
    
        if (result.success) {
            fprintf(stdout, "[Test][Fn: \"%s\"][Case: %zu]: Passed!\n", 
                t->func_name, t->case_number);
            continue;
        } 
        
        fprintf(stderr, "[Test][Fn: \"%s\"][Case: %zu]: Failed\n\tExpected:\n\t\t`%s`\n\tBut got:\n\t\t`%s`\n", 
            t->func_name, t->case_number, result.expected, result.actual);
        
        dlclose(handle); 
        return false;
    }
    
    dlclose(handle); 
    return true;
}

void try_load_files_and_test(const char *directory) 
{
    struct dirent *entry = NULL;

    DIR *dir = opendir(directory);

    if (!dir) {
        fprintf(stderr, "[Test][Error]: Could not open a directory \"%s\" with files to test: %s\n",
            directory, strerror(errno));
        exit(1);
    }

    while (entry = readdir(dir)) {
        if (entry->d_type == DT_DIR) continue;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        size_t file_name_len = strlen(entry->d_name);
        size_t directory_len = strlen(directory);

        size_t file_path_len = file_name_len + directory_len + 2;

        char *file_path = (char *)malloc(sizeof(char) * file_path_len);

        if (!file_path) {
            fprintf(stderr, "[Test][Error]: Failed to allocate memory while testing.\n");
            exit(1);
        }

        memcpy(file_path, directory, directory_len);
        memcpy(file_path + directory_len, "/", 1);
        memcpy(file_path + directory_len + 1, entry->d_name, file_name_len);
        memcpy(file_path + directory_len + 1 + file_name_len, "\0", 1);

        bool test_result = try_load_file_and_test(file_path);

        free(file_path);

        if (!test_result) {
            closedir(dir);
            exit(1);
        }
    }

    closedir(dir);
}