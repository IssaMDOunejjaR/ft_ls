#include "lib.h"

void ft_putstr_fd(int fd, char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    ft_putchar_fd(fd, str[i]);
  }
}
