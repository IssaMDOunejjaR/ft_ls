#include "../ft_ls.h"

void print_dir_content(DIR *dir, t_params *params, char *dirname) {
  struct dirent *dir_content;

  size_t param_len = tab_len(params->paths);

  if (param_len > 1) {
    ft_putstr_fd(1, dirname);
    ft_putendl_fd(1, ":");
  }

  while ((dir_content = readdir(dir))) {
    ft_putstr_fd(1, dir_content->d_name);
    ft_putchar_fd(1, '\t');
  }

  ft_putchar_fd(1, '\n');

  if (param_len > 1) {
    ft_putchar_fd(1, '\n');
  }
}
