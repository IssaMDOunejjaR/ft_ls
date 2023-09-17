#include "../ft_ls.h"

void no_dir_error(char *name) {
  ft_putstr_fd(2, "ft_ls: cannot access '");
  ft_putstr_fd(2, name);
  ft_putendl_fd(2, "': No such file or directory");
}
