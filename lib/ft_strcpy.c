#include "lib.h"

char *ft_strcpy(char *dst, const char *src) {
  if (!dst || !src)
    return dst;

  int i = 0;

  for (i = 0; src[i] != '\0'; i++) {
    dst[i] = src[i];
  }

  dst[i] = '\0';

  return dst;
}
