#include "../ft_ls.h"

void init_params(t_params *params) {
  params->paths = malloc(sizeof(char *));
  params->paths[0] = NULL;

  params->options = malloc(sizeof(char));
}

void parse_params(t_params *params, int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      parse_options(params, argv[i]);
    } else {
      params->paths = tab_add(params->paths, ft_strdup(argv[i]));
    }
  }
}

void free_params(t_params *params) {
  free_tab(params->paths);
  free(params->options);
}
