#include "../headers/ft_lib.h"

void **ft_bubble_sort(void **list, CompareFunc cmp) {
  bool is_sorted = true;

  while (is_sorted) {
    is_sorted = false;

    for (int i = 0; list[i] != NULL; i++) {
      if (list[i + 1] != NULL && cmp(list[i], list[i + 1])) {
        void *tmp = list[i];

        list[i] = list[i + 1];
        list[i + 1] = tmp;

        is_sorted = true;
      }
    }
  }

  return list;
}
