#include "../headers/ft_lib.h"

char **table_push(char **table, char *value) {
  char **new_table = NULL;
  size_t len = table_length(table);

  new_table = malloc(sizeof(char *) * (len + 2));

  if (table != NULL) {
    for (size_t i = 0; i < len; i++)
      new_table[i] = ft_strdup(table[i]);
  }

  new_table[len] = value;
  new_table[len + 1] = NULL;

  table_free(table);

  return new_table;
}
