#include "lib.h"

void free_tab(char **tab) {
  for (int i = 0; tab[i] != NULL; i++) {
    free(tab[i]);
  }

  free(tab);
}
