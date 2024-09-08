#include "../headers/ft_lib.h"

void ft_putnbr(int number) {
  if (number < 0) {
    ft_putchar('-');
    number = -number;
  }

  if (number > 9)
    ft_putnbr(number / 10);

  ft_putchar(number % 10 + '0');
}
