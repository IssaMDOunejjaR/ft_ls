#include "../headers/ft_lib.h"

int ft_putendl(char *str) {
  int count = 0;

  count += ft_putstr(str);
  count += ft_putchar('\n');

  return count;
}
