#pragma once

#include <cstdio>

#include "zipfile.hpp"

zipfile_t zip_open_file(const char *path);

zipfile_t zip_open_fp(FILE *fp);

zipfile_t zip_open_fd(int fd);

int zip_get_entry_count(zipfile_t zip);

static inline zipfile_t zip_open_buffer(const char *buffer, size_t sz) {
    return init_zipfile((void *) buffer, sz);
}

static inline void zip_close(zipfile_t zip) {
    release_zipfile(zip);
}

static inline zipentry_t zip_foreach(zipfile_t file, void **cookie) {
    return iterate_zipfile(file, cookie);
}

static inline zipentry_t zip_lookup(zipfile_t zip, const char *name) {
    return lookup_zipentry(zip, name);
}

bool zipentry_extract_to(zipentry_t entry, const char *dir);

static inline size_t zipentry_get_size(zipentry_t e) {
    return get_zipentry_size(e);
}

static inline char *zipentry_get_name(zipentry_t e) {
    return get_zipentry_name(e);
}

static inline int zipentry_decompress(zipentry_t entry, void *buf, int bufsize) {
    return decompress_zipentry(entry, buf, bufsize);
}
