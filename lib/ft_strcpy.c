#include "../headers/ft_lib.h"

char *ft_strcpy(char *dst, const char *src) {
  int i = 0;

  while (src[i] != '\0') {
    dst[i] = src[i];

    i++;
  }

  return dst;
}
