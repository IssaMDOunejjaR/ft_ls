#include "../headers/ft_lib.h"

size_t ft_number_len(int number) {
  size_t len = 0;

  if (number == 0)
    return 1;

  if (number < 0) {
    len++;
    number = -number;
  }

  while (number > 0) {
    len++;

    number /= 10;
  }

  return len;
}
