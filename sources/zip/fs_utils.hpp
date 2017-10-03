#pragma once

#include <stdbool.h>
#include <unistd.h>

bool mkdir_recursive(const char *path, mode_t mode);

bool mkdir_parent(const char *path, mode_t mode);
