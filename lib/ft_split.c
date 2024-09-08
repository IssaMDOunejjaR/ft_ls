#include "../headers/ft_lib.h"
#include <stdio.h>

char **ft_split(const char *str, char c) {
  size_t count_char = 0;

  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == c) {
      count_char++;

      while (str[i] == c && str[i] != '\0')
        i++;
    }
  }

  size_t arr_len = count_char + 2;
  char **arr = malloc(sizeof(char *) * arr_len);
  int i = 0, j = 0;

  while (arr_len > 1) {
    int idx = ft_index_of_char(str + i, c);

    arr[j] = ft_substr(str + i, 0, idx - 1);

    i += idx;

    while (str[i] == c && str[i] != '\0')
      i++;

    arr_len--;
    j++;
  }

  arr[j] = NULL;

  return arr;
}
