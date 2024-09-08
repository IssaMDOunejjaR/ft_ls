#include "../headers/ft_lib.h"

int ft_putstr(char *str) {
  int count = 0;

  for (int i = 0; str[i] != '\0'; i++)
    count += ft_putchar(str[i]);

  return count;
}
