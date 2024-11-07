#include "../ft_ls.h"

void print_file_name(FileInfo *file_info, enum Filetype file_type) {
  size_t capacity = 20;
  char out[capacity];
  size_t pos = 0;

  if (print_with_color)
    output_buffering(out, &pos, capacity, filetype_color[file_type]);

  if (print_quotes && file_info->print_quote) {
    if (file_info->has_single_quote)
      output_buffering(out, &pos, capacity, "\"");
    else
      output_buffering(out, &pos, capacity, "'");
  }

  output_buffering(out, &pos, capacity, file_info->name);

  if (print_quotes && file_info->print_quote) {
    if (file_info->has_single_quote)
      output_buffering(out, &pos, capacity, "\"");
    else
      output_buffering(out, &pos, capacity, "'");
  }

  output_buffering(out, &pos, capacity, WHITE);

  ft_putstr(1, out);
}

void print_help() {
  ft_putstr(1, "Usage: ");
  ft_putstr(1, program_name);
  ft_putstr(1,
            " [OPTION]... [FILE]...\nList information about the "
            "FILEs (the current directory by default).\n\nMandatory arguments "
            "to long options are mandatory for short options too.\n");

  int start_pad = 2;
  int middle_gap = 5;
  size_t left_pad = 0;

  for (int i = 0; VALID_OPTIONS[i][0] != NULL; i++) {
    size_t len =
        ft_strlen(VALID_OPTIONS[i][0]) + ft_strlen(VALID_OPTIONS[i][1]) + 2;

    if (left_pad < len)
      left_pad = len;
  }

  left_pad += start_pad + middle_gap;

  for (int i = 0; VALID_OPTIONS[i][0] != NULL || VALID_OPTIONS[i][1] != NULL;
       i++) {
    for (int j = 0; j < start_pad; j++)
      ft_putchar(1, ' ');

    if (VALID_OPTIONS[i][0] != NULL)
      ft_putstr(1, VALID_OPTIONS[i][0]);

    if (VALID_OPTIONS[i][0] != NULL && VALID_OPTIONS[i][1] != NULL)
      ft_putstr(1, ", ");

    if (VALID_OPTIONS[i][1] != NULL)
      ft_putstr(1, VALID_OPTIONS[i][1]);

    size_t option_len = ft_strlen(VALID_OPTIONS[i][0]) +
                        ft_strlen(VALID_OPTIONS[i][1]) +
                        (VALID_OPTIONS[i][0] && VALID_OPTIONS[i][1] ? 2 : 0);

    for (size_t j = 0; j < (left_pad - option_len - start_pad) + middle_gap;
         j++)
      ft_putchar(1, ' ');

    int max_paragraph_len = 70, count = 0;

    for (int j = 0; VALID_OPTIONS[i][2][j] != '\0'; j++) {
      ft_putchar(1, VALID_OPTIONS[i][2][j]);
      count++;

      if (count == max_paragraph_len) {
        count = 0;
        ft_putchar(1, '\n');

        for (size_t k = 0; k < left_pad + middle_gap; k++)
          ft_putchar(1, ' ');
      }
    }

    ft_putchar(1, '\n');
  }
}
