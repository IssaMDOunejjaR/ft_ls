#include "lib.h"
#include <stdio.h>

char *ft_strjoin(char *s1, char *s2) {
  if (!s1 || !s2)
    return NULL;

  int s1_len = ft_strlen(s1);
  int s2_len = ft_strlen(s2);

  char *str = malloc(sizeof(char) * (s1_len + s2_len + 1));

  if (!str)
    return NULL;

  ft_strcpy(str, s1);
  ft_strcpy(&str[s1_len], s2);

  return str;
}
