#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

void print_spaces(char *str, int size) {
  char *has_space = strchr(str, ' ');

  if (has_space != NULL)
    putchar('\'');

  printf("%s", str);

  if (has_space != NULL)
    putchar('\'');

  while (size > 0) {
    printf(" ");

    size--;
  }
}

size_t list_size(void **lst) {
  size_t size = 0;

  if (lst == NULL)
    return size;

  for (int i = 0; lst[i] != NULL; i++)
    size++;

  return size;
}

void list_free(void **lst) {
  for (int i = 0; lst[i] != NULL; i++)
    free(lst[i]);

  free(lst);
}

char **list_append(char **lst, char *value) {
  char **new_list = NULL;

  if (lst == NULL) {
    new_list = malloc(sizeof(char *) * 2);

    new_list[0] = strdup(value);
    new_list[1] = NULL;

    return new_list;
  }

  new_list = malloc(sizeof(char *) * (list_size((void **)lst) + 2));
  int i;

  for (i = 0; lst[i] != NULL; i++)
    new_list[i] = strdup(lst[i]);

  new_list[i++] = strdup(value);
  new_list[i] = NULL;

  list_free((void **)lst);

  return new_list;
}

int *arraycpy(int *arr, size_t size) {
  if (arr == NULL)
    return NULL;

  int *new_arr = malloc(size);

  memcpy(new_arr, arr, size);

  return new_arr;
}

int main(int argc, char **argv) {
  DIR *o_dir;
  char *dirname = ".";

  if (argc > 1)
    dirname = argv[1];

  o_dir = opendir(dirname);

  if (o_dir == NULL) {
    write(2, "ft_ls: cannot open directory '", 30);
    write(2, dirname, strlen(dirname));
    write(2, "': ", 3);
    perror("");

    return -1;
  }

  struct winsize w;

  if (ioctl(0, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");

    return -1;
  }

  char **strings = NULL;
  struct dirent *dir;
  int num_strings = 0;
  int quote_space = 0;
  int gap = 2;

  while ((dir = readdir(o_dir))) {
    strings = list_append(strings, dir->d_name);

    if (quote_space == 0 && strchr(dir->d_name, ' ') != NULL) {
      quote_space = 1;
      gap++;
    }

    num_strings++;
  }

  int terminal_width = w.ws_col;

  int num_columns = 1;
  int num_rows = num_strings;
  int *col_widths = NULL;
  int *tmp = NULL;

  /* printf("Params:\n"); */
  /* printf(" - terminal_width: %d\n", terminal_width); */
  /* printf(" - number of string: %d\n", num_strings); */
  /**/
  /* printf("\n"); */

  while (1) {
    num_rows = (num_strings + num_columns - 1) / num_columns;
    free(tmp);
    tmp = arraycpy(col_widths, sizeof(int) * (num_columns - 1));
    col_widths = realloc(col_widths, sizeof(int) * num_columns);
    int total_width = quote_space;

    for (int col = 0; col < num_columns; col++) {
      int max_len = 0;

      for (int row = 0; row < num_rows; row++) {
        int index = row + col * num_rows;

        if (index < num_strings) {
          int len = strlen(strings[index]);

          if (strchr(strings[index], ' ') != NULL)
            len++;

          if (len > max_len) {
            max_len = len;
          }
        }
      }

      col_widths[col] = max_len;
      total_width += max_len;

      if (col < num_columns - 1) {
        total_width += gap;
      }
    }

    /* printf("total_width: %d\n", total_width); */
    /* printf("num_columns: %d\n", num_columns); */
    /* for (int i = 0; i < num_columns; i++) */
    /*   printf("[%d: %d] ", i + 1, col_widths[i]); */
    /* printf("\n"); */

    if (total_width >= terminal_width) {
      if (num_columns > 1) {
        free(col_widths);

        col_widths = tmp;
        num_columns--;
        num_rows = (num_strings + num_columns - 1) / num_columns;
      }

      break;
    } else if (num_columns == num_strings)
      break;

    num_columns++;
  }

  /* printf("\n"); */
  /**/
  /* printf(" - number of columns: %d\n", num_columns); */
  /* printf(" - number of rows: %d\n", num_rows); */
  /* printf(" - columns widths:\n"); */
  /**/
  /* for (int i = 0; i < num_columns; i++) */
  /*   printf("   - %d: %d\n", i + 1, col_widths[i]); */
  /**/
  /* printf("\n"); */

  for (int row = 0; row < num_rows; row++) {
    for (int col = 0; col < num_columns; col++) {
      int index = row + col * num_rows;

      if (index < num_strings) {
        char *is_spaced_name = strchr(strings[index], ' ');

        if (col == 0 && quote_space == 1 && is_spaced_name == NULL)
          printf(" ");

        int spaces = 0;
        int next_word = row + (col + 1) * num_rows;

        if (col < num_columns - 1) {
          spaces = col_widths[col] - strlen(strings[index]) + gap;

          if (next_word < num_strings &&
              strchr(strings[next_word], ' ') != NULL) {
            spaces--;
          }
        }

        if (is_spaced_name != NULL)
          spaces--;

        print_spaces(strings[index], spaces);
      }
    }

    printf("\n");
  }

  free(col_widths);
  list_free((void **)strings);

  closedir(o_dir);

  return 0;
}
