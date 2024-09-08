#include "../headers/ft_lib.h"

int ft_index_of_char(const char *str, char c) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (c == str[i])
      return i;
  }

  return -1;
}
