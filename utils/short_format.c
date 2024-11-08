#include "../ft_ls.h"

// TODO: improve
void calc_many_per_line_format(Input *input, FileInfo **list) {
  size_t num_columns = 1, num_rows = input->size;
  size_t *col_widths = NULL, *tmp = NULL;

  while (1) {
    num_rows = (input->size + num_columns - 1) / num_columns;

    if (col_widths != NULL) {
      if (tmp)
        free(tmp);

      size_t size = sizeof(size_t) * (num_columns - 1);

      tmp = malloc(size);

      memcpy(tmp, col_widths, size);

      free(col_widths);
    }

    col_widths = malloc(sizeof(size_t) * num_columns);

    size_t total_width = input->column_info.print_quote ? 1 : 0;

    for (size_t col = 0; col < num_columns; col++) {
      size_t max_len = 0;

      for (size_t row = 0; row < num_rows; row++) {
        size_t index = row + col * num_rows;

        if (index < input->size) {
          size_t len = ft_strlen(list[index]->name);

          if (print_quotes) {
            char *has_set = ft_has_set(list[index]->name, name_set);

            if (has_set != NULL) {
              input->column_info.print_quote = true;
              input->column_info.gap = 3;
              list[index]->print_quote = true;

              if (has_set[0] == '\'')
                list[index]->has_single_quote = true;
              else
                list[index]->has_single_quote = false;

              len++;
            } else
              list[index]->print_quote = false;
          }

          if (len > max_len) {
            max_len = len;
          }
        }
      }

      col_widths[col] = max_len;
      total_width += max_len;

      if (col < num_columns - 1) {
        total_width += input->column_info.gap;
      }
    }

    if (total_width >= input->column_info.term_width) {
      if (num_columns > 1) {
        free(col_widths);
        col_widths = tmp;
        tmp = NULL;
        num_columns--;
        num_rows = (input->size + num_columns - 1) / num_columns;
      }

      break;
    } else if (num_columns == input->size) {
      free(tmp);

      break;
    }

    num_columns++;
  }

  input->column_info.num_columns = num_columns;
  input->column_info.num_rows = num_rows;
  input->column_info.col_widths = col_widths;
}

void out_column_format(Input *input, FileInfo **list) {
  ColumnInfo column_info = input->column_info;

  for (size_t row = 0; row < column_info.num_rows; row++) {
    for (size_t col = 0; col < column_info.num_columns; col++) {
      size_t index = row + col * column_info.num_rows;

      if (index < input->size) {
        FileInfo *file_info = list[index];

        if (col == 0 && print_quotes && column_info.print_quote &&
            !file_info->print_quote)
          ft_putchar(1, ' ');

        int spaces = 0;
        size_t next_word = row + (col + 1) * column_info.num_rows;

        if (col < column_info.num_columns - 1) {
          spaces = column_info.col_widths[col] - ft_strlen(file_info->name) +
                   column_info.gap;

          if (print_quotes && next_word < input->size &&
              ft_has_set(list[next_word]->name, name_set) != NULL) {
            spaces--;
          }
        }

        if (print_quotes && file_info->print_quote)
          spaces--;

        if (next_word > input->size || !list[next_word])
          spaces = 0;

        print_dir_name(file_info, spaces);
      }
    }

    ft_putchar(1, '\n');
  }
}
