#include "../ft_ls.h"

const char ALLOWED_OPTIONS[] = {'l', 'R', 'a', 'r', 't'};

int is_allowed_option(char option) {
  for (int i = 0; i < 5; i++) {
    if (option == ALLOWED_OPTIONS[i])
      return 1;
  }

  return 0;
}

void parse_options(t_params *params, char *options) {
  for (int i = 1; options[i] != '\0'; i++) {
    if (!is_allowed_option(options[i])) {
      ft_putstr_fd(2, "ft_ls: invalid option -- ");
      ft_putchar_fd(2, '\'');
      ft_putchar_fd(2, options[i]);
      ft_putendl_fd(2, "'");
      ft_putendl_fd(2, "Try 'ft_ls --help' for more information.");

      return;
    }
  }

  params->options = ft_strjoin(params->options, &options[1]);
}
