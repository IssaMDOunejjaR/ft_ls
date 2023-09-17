#include "ft_ls.h"
#include "lib/lib.h"
#include <dirent.h>
#include <stdio.h>

int main(int argc, char **argv) {
  t_params params;

  init_params(&params);

  parse_params(&params, argc, argv);

  for (int i = 0; params.paths[i] != NULL; i++) {
    DIR *dir = opendir(params.paths[i]);

    if (!dir) {
      int fd = open(params.paths[i], O_RDONLY);

      if (fd == -1) {
        no_dir_error(params.paths[i]);
        free_params(&params);

        return -2;
      }

      ft_putendl_fd(1, params.paths[i]);

      close(fd);

      continue;
    }

    print_dir_content(dir, &params, params.paths[i]);

    closedir(dir);
  }

  ft_putendl_fd(1, "==========================");

  for (int i = 0; params.paths[i] != NULL; i++) {
    printf("%s\n", params.paths[i]);
  }

  printf("options = [%s]\n", params.options);

  return 0;
}
