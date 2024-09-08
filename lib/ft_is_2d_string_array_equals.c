#include "../headers/ft_lib.h"

int ft_is_2d_string_array_equals(char **a, char **b) {
  int i = 0;

  while (a[i] != NULL) {
    if (!ft_strcmp(a[i], b[i]))
      return 0;

    i++;
  }

  if (!ft_strcmp(a[i], b[i]))
    return 0;

  return 1;
}
