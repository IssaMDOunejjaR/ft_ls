#include "../ft_ls.h"

static size_t get_terminal_width(void) {
  struct winsize w;

  if (ioctl(0, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");

    return 0;
  }

  return w.ws_col;
}

void init_column_info(ColumnInfo *column_info) {
  column_info->print_quote = false;
  column_info->print_acl = false;

  column_info->group_width = 0;
  column_info->owner_width = 0;
  column_info->nlink_width = 0;
  column_info->size_width = 0;
  column_info->block_size = 0;

  column_info->term_width = get_terminal_width();
  column_info->num_columns = 0;
  column_info->num_rows = 0;
  column_info->gap = 2;
  column_info->col_widths = NULL;
}

void init_inputs(void) {
  all.list = NULL;
  all.size = 0;
  all.error = false;
  init_column_info(&all.column_info);

  files.list = NULL;
  files.size = 0;
  files.error = false;
  init_column_info(&files.column_info);

  dirs.list = NULL;
  dirs.size = 0;
  dirs.error = false;
  init_column_info(&dirs.column_info);
}
