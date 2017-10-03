#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>

#include "fs_utils.hpp"
#include "string_utils.hpp"

#ifndef PATH_MAX
#define PATH_MAX 1024 /* posix min */
#endif

bool mkdir_recursive(const char *cpath, mode_t mode) {
    char *path = strdup(cpath);
    if (!path) {
        return false;
    }

    char currpath[PATH_MAX];
    char *pathpiece;

    struct stat st;
    strcpy(currpath, "");
    pathpiece = strtok(path, "/");

    if (path[0] == '/') {
        strcat(currpath, "/");
    }

    while (pathpiece != nullptr) {
        if (strlen(currpath) + strlen(pathpiece) + 2 > PATH_MAX) {
            free(path);
            return false;
        }

        strcat(currpath, pathpiece);
        strcat(currpath, "/");

        if (stat(currpath, &st) != 0) {
            if (mkdir(currpath, mode) < 0) {
                free(path);
                return false;
            }
        }

        pathpiece = strtok(nullptr, "/");
    }

    free(path);
    return true;
}

bool mkdir_parent(const char *cpath, mode_t mode) {
    if (!cpath) {
        return false;
    }

    char *path = strdup(cpath);
    if (!path) {
        return false;
    }

    char *slash = strrchr(path, '/');
    if (!slash) {
        free(path);
        return false;
    }

    // split path and file name
    *slash = '\0';

    if (access(path, F_OK) == 0) {
        free(path);
        return true;
    }

    // create parent directory
    bool ret = mkdir_recursive(path, mode);

    free(path);
    return ret;
}
