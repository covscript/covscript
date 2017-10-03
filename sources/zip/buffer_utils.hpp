#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>

char* buffer_readall_fd(int fd, ssize_t *psz);
