#include "../headers/ft_lib.h"

char *ft_to_lowercase(char *str) {
  char *new_str = malloc(sizeof(char) * ft_strlen(str) + 1);
  int i;

  for (i = 0; str[i] != '\0'; i++)
    new_str[i] = ft_to_lower(str[i]);

  new_str[i] = '\0';

  return new_str;
}
