#include "string_utils.hpp"

#include <cstdlib>
#include <cstring>

char* string_concat(const char *l, ...)
{
  if (!l) {
    return nullptr;
  }
  
  size_t sz = strlen(l) + 1; // '\0'
  char *str = (char*) malloc(sizeof(char) * sz);
  if (!str) {
    return nullptr;
  }
  
  memset(str, '\0', sz);
  strcpy(str, l);
  
  va_list arg;
  va_start(arg, l);
  
  size_t psz = 0;
  const char *p = nullptr;
  while ((p = va_arg(arg, const char*))) {
    psz = strlen(p);
    if (psz == 0) {
      continue;
    }
    
    sz += psz;
    str = (char*) realloc(str, sizeof(char) * sz);
    if (!str) {
      goto fail;
    }
    
    strcat(str, p);
    str[sz - 1] = '\0';
  }
  
  va_end(arg);
  return str;
  
fail:
  if (str) {
    free(str);
  }
  va_end(arg);
  return nullptr;
}
