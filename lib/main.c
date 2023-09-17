#include "lib.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  char **tab = malloc(sizeof(char *));

  tab[0] = NULL;

  tab = tab_add(tab, "Hello");
  tab = tab_add(tab, "hi");
  tab = tab_add(tab, "Issam");
  tab = tab_add(tab, "Ounejjar");

  for (int i = 0; tab[i] != NULL; i++) {
    printf("%s\n", tab[i]);
  }

  return EXIT_SUCCESS;
}
