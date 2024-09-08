#include "../headers/ft_lib.h"

int ft_last_index_of(char *str, char c) {
  for (int i = ft_strlen(str) - 1; i >= 0; i--) {
    if (str[i] == c)
      return i;
  }

  return -1;
}
