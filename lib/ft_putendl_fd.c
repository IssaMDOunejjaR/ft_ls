#include "lib.h"

void ft_putendl_fd(int fd, char *str) {
  ft_putstr_fd(fd, str);
  ft_putchar_fd(fd, '\n');
}
