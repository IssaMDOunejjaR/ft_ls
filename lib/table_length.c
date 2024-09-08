#include "../headers/ft_lib.h"

size_t table_length(char **table) {
  if (table == NULL)
    return 0;

  size_t len = 0;

  for (int i = 0; table[i] != NULL; i++)
    len++;

  return len;
}
