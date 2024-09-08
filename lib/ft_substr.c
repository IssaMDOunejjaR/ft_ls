#include "../headers/ft_lib.h"
#include <stdio.h>
#include <stdlib.h>

char *ft_substr(const char *str, size_t start, size_t end) {
  if (str == NULL || start > end)
    return NULL;

  size_t str_len = ft_strlen(str);
  size_t st = start > str_len ? str_len : start;
  size_t ed = end > str_len ? str_len : end;

  char *substr = malloc(sizeof(char) * ((ed - st) + 1));

  if (!substr)
    return NULL;

  int i = 0;

  for (size_t j = st; j <= ed; j++) {
    substr[i] = str[j];

    i++;
  }

  substr[i] = '\0';

  return substr;
}
