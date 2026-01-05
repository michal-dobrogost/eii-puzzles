#pragma once
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read an entire FILE stream into a string.
 * @return NULL on error. User is responsible for free().
 */
static char* fileToStr(FILE* stream) {
  char* buffer = NULL;
  size_t len;
  ssize_t strLen = getdelim(&buffer, &len, '\0', stream);
  if (strLen == -1) {
    return NULL;
  }
  return buffer;
}

#ifdef __cplusplus
} // extern "C"
#endif
