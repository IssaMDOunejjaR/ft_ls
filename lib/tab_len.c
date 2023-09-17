#include "lib.h"

size_t tab_len(char **tab) {
  size_t len = 0;

  for (int i = 0; tab[i] != NULL; i++)
    len += 1;

  return len;
}
