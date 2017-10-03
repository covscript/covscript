#include "buffer_utils.hpp"

#define TYPE_SIZE sizeof(char)

char* buffer_readall_fd(int fd, ssize_t *psz)
{
  ssize_t sz = 2; // '\0'
  char buffer[1];
  char *ret = (char*) malloc(TYPE_SIZE * sz);
  
  if (!ret) {
    return nullptr;
  }
  
  while (read(fd, buffer, 1) == 1) {
    ret[sz-2] = buffer[0];
    ret[sz-1] = '\0';
    
    sz++;
    ret = (char*) realloc(ret, TYPE_SIZE * sz);
    if (!ret) {
      return nullptr;
    }
  }
  
  if (psz) {
    *psz = sz;
  }
  
  return ret;
}
