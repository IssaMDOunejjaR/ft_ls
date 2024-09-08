#include "../headers/ft_lib.h"

char *ft_strjoin(char *s1, char *s2) {
  if (s1 == NULL)
    return NULL;

  if (s2 == NULL)
    return ft_strdup(s1);

  size_t s1_len = ft_strlen(s1);
  size_t s2_len = ft_strlen(s2);
  char *str = malloc(sizeof(char) * (s1_len + s2_len + 1));

  ft_strcpy(str, s1);
  ft_strcpy(str + s1_len, s2);

  str[s1_len + s2_len] = '\0';

  return str;
}
