#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <fcntl.h>
#include <zconf.h>
#include "quicker_zip.hpp"
#include "zip.hpp"
#include "fs_utils.hpp"
#include "buffer_utils.hpp"

zipfile_t zip_open_file(const char *path)
{
  if (!path) {
    return nullptr;
  }
  
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    return nullptr;
  }
  
  zipfile_t ret = zip_open_fd(fd);
  close(fd);
  return ret;
}

zipfile_t zip_open_fp(FILE *fp)
{
  if (!fp) {
    return nullptr;
  }
  
  return zip_open_fd(fileno(fp));
}

zipfile_t zip_open_fd(int fd)
{
  ssize_t sz;
  char *buffer = buffer_readall_fd(fd, &sz);
  if (!buffer) {
    return nullptr;
  }
  
  zipfile_t info = zip_open_buffer(buffer, (size_t) sz);
  if (!info) {
    return nullptr;
  }
  
  free(buffer);
  return info;
}

int zip_get_entry_count(zipfile_t zip)
{
  CZipFile *z = (CZipFile*) zip;
  return z ? z->entryCount : 0;
}

bool zipentry_extract_to(zipentry_t entry, const char *dir)
{
  if (!entry || !dir) {
    return false;
  }
  
  const char *name = zipentry_get_name(entry);
  size_t esz = zipentry_get_size(entry);
  
  size_t name_len = strlen(name);
  size_t dir_len = strlen(dir);
  if (name_len == 0 || dir_len == 0) {
    return false;
  }
  
  size_t total = name_len + dir_len + 1;
  bool increased = false;
  
  if (dir[dir_len - 1] != '/') {
    total++;
    increased = true;
  }
  
  char *dest_path = (char*) malloc(sizeof(char) * total);
  if (!dest_path) {
    return false;
  }
  
  memset(dest_path, '\0', total);
  strcat(dest_path, dir);
  if (increased) {
    // make sure that at least one slash in path string
    strcat(dest_path, "/");
  }
  strcat(dest_path, name);
  
  if (name[name_len - 1] == '/' && esz == 0) {
    // Is a directory, just create it.
    bool r = mkdir_recursive(dest_path, 0755);
    free(dest_path);
    return r;
  }

  char *ebuf = nullptr;
  FILE *dest = nullptr;
  
  if (!mkdir_parent(dest_path, 0755)) {
    goto fail;
  }
  
  dest = fopen(dest_path, "wb");
  if (!dest) {
    goto fail;
  }

  ebuf = (char*) malloc(sizeof(char) * (esz + 1));
  if (!ebuf) {
    goto fail_file;
  }
  
  zipentry_decompress(entry, ebuf, esz);
  ebuf[esz] = '\0';
  
  fwrite(ebuf, esz, 1, dest);
  
  fclose(dest);
  free(dest_path);
  return true;

fail_file:
  if (dest) {
    fclose(dest);
  }
fail:
  if (dest_path) {
    free(dest_path);
  }
  return false;
}
