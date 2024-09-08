#include "../headers/ft_lib.h"
#include <stdio.h>
#include <stdlib.h>

static int is_in_set(char c, char const *set) {
  for (int i = 0; set[i] != '\0'; i++) {
    if (set[i] == c)
      return 1;
  }

  return 0;
}

char *ft_trim(char const *str, char const *set) {
  if (str == NULL || set == NULL)
    return NULL;

  int begin = 0, end = 0;

  for (int i = 0; str[i] != '\0'; i++) {
    if (is_in_set(str[i], set) == 0) {
      begin = i;

      break;
    }
  }

  for (int i = ft_strlen(str) - 1; i >= 0; i--) {
    if (is_in_set(str[i], set) == 0) {
      end = i;

      break;
    }
  }

  char *trim = malloc(sizeof(char) * ((end - begin) + 1));
  int i = 0;

  while (begin <= end)
    trim[i++] = str[begin++];

  trim[i] = '\0';

  return trim;
}
