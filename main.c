#include "ft_ls.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct s_params {
  char **paths;
  char *options;
} t_params;

const char ALLOWED_OPTIONS[] = {'l', 'R', 'a', 'r', 't'};

int is_allowed_option(char option) {
  for (int i = 0; i < 5; i++) {
    if (option == ALLOWED_OPTIONS[i])
      return 1;
  }

  return 0;
}

void print_list(t_params *params, char *options) {}

t_params *parse_params(int argc, char **argv) {
  t_params *params = malloc(sizeof(t_params));

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
    }
  }

  return params;
}

int main(int argc, char **argv) {
  t_params *params = parse_params(argc, argv);

  free(params);

  return 0;
}
