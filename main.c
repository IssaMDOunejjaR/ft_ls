#include "ft_ls.h"

int exit_status = 0;
char *program_name;

// Options
bool sort_reverse;
bool print_owner = true;
bool print_group = true;
bool print_with_color;
bool recursive;
bool immediate_dirs;
bool ignore_hidden_files = true;
bool ignore_dots;
bool print_quotes = true;
bool print_access_time = false;
bool comma_separated;
bool write_slash;
bool one_column;
bool _u;

char filetype_letter[] = "d-l-";

char *filetype_color[] = {DIRECTORY_COLOR, FILE_COLOR, SYMLINK_COLOR,
                          EXECUTABLE_COLOR, STICKY_BIT_COLOR};
Input all, files, dirs;

char *name_set = " $'\"[]=()!";

OwnerCache *owner_cache = NULL;
GroupCache *group_cache = NULL;

enum SortType sort_type = sort_name;
enum Format format = many_per_line;

int main(int argc, char *argv[]) {
  init_inputs();

  program_name = argv[0];

  if (parse_args(argc, argv) == 0) {
    sort_inputs(&all);
    process_inputs();
  }

  clear_inputs();

  return exit_status;
}
