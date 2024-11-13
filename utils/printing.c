#include "../ft_ls.h"

void print_short_option_not_found(char option) {
  char *prefix = ": invalid option -- '";
  char *suffix = "'\nTry '";
  char *end = " --help' for more information.\n";

  size_t program_name_len = ft_strlen(program_name);
  size_t prefix_len = ft_strlen(prefix);
  size_t option_len = 1;
  size_t suffix_len = ft_strlen(suffix);
  size_t end_len = ft_strlen(end);

  char *msg =
      malloc(sizeof(char) * (program_name_len + prefix_len + option_len +
                             suffix_len + program_name_len + end_len + 1));

  if (msg == NULL)
    return;

  ft_strcpy(msg, program_name);
  ft_strcpy(msg + program_name_len, prefix);
  ft_strcpy(msg + program_name_len + prefix_len, &option);
  ft_strcpy(msg + program_name_len + prefix_len + option_len, suffix);
  ft_strcpy(msg + program_name_len + prefix_len + option_len + suffix_len,
            program_name);
  ft_strcpy(msg + program_name_len + prefix_len + option_len + suffix_len +
                program_name_len,
            end);

  msg[program_name_len + prefix_len + option_len + suffix_len +
      program_name_len + end_len] = '\0';

  ft_putstr(2, msg);

  free(msg);
}

void print_long_option_not_found(char *option) {
  char *prefix = ": unrecognized option '";
  char *suffix = "'\nTry '";
  char *end = " --help' for more information.\n";

  size_t program_name_len = ft_strlen(program_name);
  size_t prefix_len = ft_strlen(prefix);
  size_t option_len = ft_strlen(option);
  size_t suffix_len = ft_strlen(suffix);
  size_t end_len = ft_strlen(end);

  char *msg =
      malloc(sizeof(char) * (program_name_len + prefix_len + option_len +
                             suffix_len + program_name_len + end_len + 1));

  if (msg == NULL)
    return;

  ft_strcpy(msg, program_name);
  ft_strcpy(msg + program_name_len, prefix);
  ft_strcpy(msg + program_name_len + prefix_len, option);
  ft_strcpy(msg + program_name_len + prefix_len + option_len, suffix);
  ft_strcpy(msg + program_name_len + prefix_len + option_len + suffix_len,
            program_name);
  ft_strcpy(msg + program_name_len + prefix_len + option_len + suffix_len +
                program_name_len,
            end);

  msg[program_name_len + prefix_len + option_len + suffix_len +
      program_name_len + end_len] = '\0';

  ft_putstr(2, msg);

  free(msg);
}

void input_not_found(char *input) {
  char *prefix = ": cannot access '";
  char *suffix = "': No such file or directory\n";

  size_t program_name_len = ft_strlen(program_name);
  size_t prefix_len = ft_strlen(prefix);
  size_t option_len = ft_strlen(input);
  size_t suffix_len = ft_strlen(suffix);

  char *msg = malloc(sizeof(char) * (program_name_len + prefix_len +
                                     option_len + suffix_len + 1));

  if (msg == NULL)
    return;

  ft_strcpy(msg, program_name);
  ft_strcpy(msg + program_name_len, prefix);
  ft_strcpy(msg + program_name_len + prefix_len, input);
  ft_strcpy(msg + program_name_len + prefix_len + option_len, suffix);

  msg[program_name_len + prefix_len + option_len + suffix_len] = '\0';

  ft_putstr(2, msg);

  free(msg);
}

void print_block_size(unsigned long size) {
  ft_putstr(1, "total ");
  ft_putnbr(1, size / 2);
  ft_putstr(1, "\n");
}

void print_error(char *input, char *message) {
  ft_putstr(1, program_name);
  ft_putstr(1, ": ");
  ft_putstr(1, message);
  ft_putstr(1, " '");
  ft_putstr(1, input);
  ft_putstr(1, "': ");
  ft_putstr(1, strerror(errno));
  ft_putchar(1, '\n');
}

static void print_file_name(FileInfo *file_info, enum Filetype file_type) {
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

void print_dir_name(FileInfo *file_info, int size) {
  print_file_name(file_info, file_info->filetype);

  while (size > 0) {
    ft_putchar(1, ' ');

    size--;
  }
}

void print_current_dir_name(char *name) {
  char *has_set = ft_has_set(name, name_set);
  bool print_quote = print_quotes && has_set != NULL;

  if (print_quote) {
    if (has_set[0] == '\'')
      ft_putchar(1, '"');
    else
      ft_putchar(1, '\'');
  }

  ft_putstr(1, name);

  if (print_quote) {
    if (has_set[0] == '\'')
      ft_putchar(1, '"');
    else
      ft_putchar(1, '\'');
  }

  ft_putstr(1, ":\n");
}

void print_out_format(Input input) {
  if (format == long_format) {
    calc_long_format(&input);
    if (!immediate_dirs)
      print_block_size(input.column_info.block_size);
    out_long_format(&input);
  } else if (format == many_per_line) {
    FileInfo **list = input_list_to_table(input);

    calc_many_per_line_format(&input, list);
    out_column_format(&input, list);

    clear_column_info(&input.column_info);
    free(list);
  }
}
