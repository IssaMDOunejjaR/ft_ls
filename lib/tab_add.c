#include "lib.h"

char **tab_add(char **tab, char *new_element) {
  size_t len = tab_len(tab);
  char **new_tab = malloc(sizeof(char *) * (len + 2));

  if (!new_tab)
    return NULL;

  int i;

  for (i = 0; tab[i] != NULL; i++) {
    new_tab[i] = ft_strdup(tab[i]);

    if (!new_tab[i]) {
      free_tab(new_tab);

      return NULL;
    }
  }

  new_tab[i++] = new_element;
  new_tab[i] = NULL;

  free(tab);

  return new_tab;
}
