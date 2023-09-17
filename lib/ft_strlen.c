#include "lib.h"

size_t ft_strlen(char *str) {
  size_t count = 0;

  for (int i = 0; str[i] != '\0'; i++)
    count += 1;

  return count;
}
