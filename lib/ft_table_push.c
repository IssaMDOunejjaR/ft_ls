#include "../headers/ft_lib.h"

char **table_push(char **table, char *value) {
  char **new_table = NULL;

  if (table == NULL) {
    new_table = malloc(sizeof(char *) * 2);

    new_table[0] = value;
    new_table[1] = NULL;

    return new_table;
  }

  new_table = malloc(sizeof(table) * (table_length(table) + 2));

  int i = 0;

  while (table[i] != NULL) {
    new_table[i] = ft_strdup(table[i]);

    i++;
  }

  new_table[i++] = value;
  new_table[i] = NULL;

  table_free(table);

  return new_table;
}
