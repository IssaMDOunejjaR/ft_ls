#include "../headers/ft_lib.h"

void table_free(char **table) {
  if (table == NULL)
    return;

  for (int i = 0; table[i] != NULL; i++)
    free(table[i]);

  free(table);
}
