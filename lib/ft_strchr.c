#include "../headers/ft_lib.h"

char *ft_strchr(const char *s, int c) {
  for (int i = 0; s[i] != '\0'; i++) {
    if (s[i] == c)
      return (char *)&s[i];
  }

  return NULL;
}
