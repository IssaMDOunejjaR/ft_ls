#include "../headers/ft_lib.h"
#include <unistd.h>

void ft_putstr(char *str) { write(1, str, ft_strlen(str)); }
