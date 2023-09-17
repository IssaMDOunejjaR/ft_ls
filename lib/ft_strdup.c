#include "lib.h"

char *ft_strdup(char *str) {
  if (!str)
    return NULL;

  char *new_str = malloc(sizeof(char) * (ft_strlen(str) + 1));

  if (!new_str)
    return NULL;

  ft_strcpy(new_str, str);

  return new_str;
}
