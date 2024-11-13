#include "../ft_ls.h"

static char *VALID_OPTIONS[][4] = {
#ifdef __APPLE__
    {"-@", NULL,
     "Display extended attribute keys and sizes in long (-l) output.", NULL},
    {"-A", NULL,
     "List all entries except for . and ...  Always set for the super-user.",
     NULL},
    {"-a", NULL, "Include directory entries whose names begin with a dot (.).",
     NULL},
    {"-d", NULL,
     "Directories are listed as plain files (not searched recursively).", NULL},
    {"-f", NULL, "Output is not sorted.  This option turns on the -a option.",
     NULL},
    {"-G", NULL, "Enable colorized output.", NULL},
    {"-g", NULL,
     "This option is only available for compatibility with POSIX; it is used "
     "to display the group name in the long (-l) format output (the owner name "
     "is suppressed).",
     NULL},
    {"-l", NULL,
     "(The lowercase letter ``ell''.)  List in long format.  (See below.)  If "
     "the output is to a terminal, a total sum for all the file sizes is "
     "output on a line before the long listing.",
     NULL},
    {"-o", NULL, "List in long format, but omit the group id.", NULL},
    {"-R", NULL, "Recursively list subdirectories encountered.", NULL},
    {"-r", NULL,
     "Reverse the order of the sort to get reverse lexicographical order or "
     "the oldest entries first (or largest files last, if combined with sort "
     "by size",
     NULL},
    {"-S", NULL, "Sort files by size", NULL},
    {"-t", NULL,
     "Sort by time modified (most recently modified first) before sorting the "
     "operands by lexicographical order.",
     NULL},
    {"-u", NULL,
     "Use time of last access, instead of last modification of the file for "
     "sorting (-t) or long printing (-l).",
     NULL},
    {"-U", NULL,
     "Use time of file creation, instead of last modification for sorting (-t) "
     "or long output (-l).",
     NULL},
#endif /* ifdef __APPLE__ */

#ifdef __linux
    {"-A", "--almost_all", "do not list implied . and ..", NULL},
    {"-a", "--all", "do not ignore entries starting with .", NULL},
    {NULL, "--author", "with -l, print the author of each file", NULL},
    {NULL, "--color", "color the output", NULL},
    {"-d", "--directory", "list directories themselves, not their contents",
     NULL},
    {"-f", NULL, "do not sort, enable -aU, disable -ls --color", NULL},
    {"-G", "--no-group", "in a long listing, don't print group names", NULL},
    {"-g", NULL, "like -l, but do not list owner", NULL},
    {"-l", NULL, "use a long listing format", NULL},
    {"-N", "--literal", "print entry names without quoting", NULL},
    {"-o", NULL, "like -l, but do not list group information", NULL},
    {"-R", "--recursive", "list subdirectories recursively", NULL},
    {"-r", "--reverse", "reverse order while sorting", NULL},
    {"-S", NULL, "sort by file size, largest first", NULL},
    {"-t", NULL, "sort by time, newest first; see --time", NULL},
    {"-u", NULL,
     "with  -lt: sort by, and show, access time; with -l: show access time "
     "and "
     "sort by name; otherwise: sort by acces time, newest first",
     NULL},
    {"-U", NULL, "do not sort; list entries in directory order", NULL},
#endif /* ifdef __linux */

    {NULL, NULL},
};

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

static char set_option_macos(char c, char *opt) {
  if (opt != NULL) {
    if (ft_strcmp(opt, "--color") == 0) {
      print_with_color = true;
    } else
      return 0;

    return 1;
  } else if (c == 'a') {
    ignore_hidden_files = false;
    ignore_dots = false;
  } else if (c == 'A') {
    ignore_hidden_files = false;
    ignore_dots = true;
  } else if (c == 'd') {
    immediate_dirs = true;
  } else if (c == 'f') {
    sort_type = sort_none;
    print_with_color = false;
    ignore_hidden_files = false;
    ignore_dots = false;
  } else if (c == 'g') {
    print_owner = false;
    format = long_format;
  } else if (c == 'l') {
    format = long_format;
  } else if (c == 'o') {
    print_group = false;
    format = long_format;
  } else if (c == 'r') {
    if (sort_type != not_sort)
      sort_reverse = true;
  } else if (c == 'R') {
    recursive = true;
  } else if (c == 'S') {
    sort_type = sort_size;
  } else if (c == 't') {
    if (_u)
      sort_type = sort_access_time;
    else
      sort_type = sort_update_time;
  } else if (c == 'u') {
    print_access_time = true;
    if (sort_type != not_sort && sort_type != sort_size)
      sort_type = sort_access_time;
    _u = true;
  } else if (c == 'U') {
    sort_type = not_sort;
    sort_reverse = false;
  } else if (c == 'G') {
    print_with_color = true;
  } else {
    return 0;
  }

  return c;
}

// TODO: improve
static char set_option(char c, char *opt) {
  if (opt != NULL) {
    if (ft_strcmp(opt, "--color") == 0) {
      print_with_color = true;
    } else if (ft_strcmp(opt, "--literal") == 0) {
      print_quotes = false;
    } else if (ft_strcmp(opt, "--almost_all") == 0) {
      ignore_hidden_files = false;
      ignore_dots = true;
    } else
      return 0;

    return 1;
  } else if (c == 'a') {
    ignore_hidden_files = false;
    ignore_dots = false;
  } else if (c == 'A') {
    ignore_hidden_files = false;
    ignore_dots = true;
  } else if (c == 'd') {
    immediate_dirs = true;
  } else if (c == 'f') {
    sort_type = sort_none;
    print_with_color = false;
    format = many_per_line;
    ignore_hidden_files = false;
    ignore_dots = false;
  } else if (c == 'g') {
    print_owner = false;
    format = long_format;
  } else if (c == 'l') {
    format = long_format;
  } else if (c == 'N') {
    print_quotes = false;
  } else if (c == 'o') {
    print_group = false;
    format = long_format;
  } else if (c == 'r') {
    if (sort_type != not_sort)
      sort_reverse = true;
  } else if (c == 'R') {
    recursive = true;
  } else if (c == 'S') {
    sort_type = sort_size;
  } else if (c == 't') {
    if (_u)
      sort_type = sort_access_time;
    else
      sort_type = sort_update_time;
  } else if (c == 'u') {
    print_access_time = true;
    if (sort_type != not_sort && sort_type != sort_size)
      sort_type = sort_access_time;
    _u = true;
  } else if (c == 'U') {
    sort_type = not_sort;
    sort_reverse = false;
  } else if (c == 'G') {
    print_group = false;
  } else {
    return 0;
  }

  return c;
}

int get_opt(char short_opt, char *long_opt) {
  for (int i = 0; VALID_OPTIONS[i][0] != NULL || VALID_OPTIONS[i][1] != NULL;
       i++) {
    if (long_opt != NULL) {
      char *tmp = VALID_OPTIONS[i][1];

      if (tmp == NULL)
        continue;

      if (ft_strcmp(long_opt, tmp) == 0) {
        if (VALID_OPTIONS[i][0] == NULL)
          return set_option(0, tmp);

        return set_option(VALID_OPTIONS[i][0][1], NULL);
      }
    } else {
      char *tmp = VALID_OPTIONS[i][0];

      if (tmp == NULL)
        continue;

      if (short_opt == tmp[1]) {
#ifdef __APPLE__
        return set_option_macos(tmp[1], NULL);
#endif /* ifdef __APPLE__ */

#ifdef __linux
        return set_option(tmp[1], NULL);
#endif /* ifdef __linux */
      }
    }
  }

  return 0;
}

// TODO: improve
int parse_args(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (ft_strcmp(argv[i], "--help") == 0) {
      print_help();

      return 1;
    } else if (argv[i][0] == '-') {
      int ret = 0;

      if (argv[i][1] == '-') {
        if ((ret = get_opt(0, argv[i])) == 0)
          print_long_option_not_found(argv[i]);
      } else {
        for (int j = 1; argv[i][j] != '\0'; j++) {
          if ((ret = get_opt(argv[i][j], NULL)) == 0)
            print_short_option_not_found(argv[i][j]);
        }
      }

      if (ret == 0) {
        exit_status = 2;
        return exit_status;
      }
    } else {
      struct stat *stat = malloc(sizeof(struct stat));

      if (lstat(argv[i], stat) == -1) {
        input_not_found(argv[i]);
        all.error = true;
      } else {
        FileInfo *info = create_file_info(ft_strdup(argv[i]), stat);

        char *has_set = ft_has_set(argv[i], name_set);

        if (has_set != NULL) {
          info->print_quote = true;

          if (has_set[0] == '\'')
            info->has_single_quote = true;
          else
            info->has_single_quote = false;

          if (!all.column_info.print_quote) {
            all.column_info.print_quote = true;
            all.column_info.gap = 3;
          }
        }

        ft_list_push_back(&all.list, info);
        all.size++;
      }
    }
  }

  if (_u) {
    if (sort_type != not_sort && sort_type != sort_size) {
      if (format == long_format) {
        if (sort_type == sort_update_time)
          sort_type = sort_access_time;
        else
          sort_type = sort_name;
      }
      /* else sort_type = sort_access_time; */
    }

    /* if (format == long_format && sort_type == sort_update_time) { */
    /*   print_access_time = true; */
    /*   sort_type = sort_access_time; */
    /* } else if (format == long_format) { */
    /*   print_access_time = true; */
    /*   sort_type = sort_name; */
    /* } else { */
    /*   sort_type = sort_access_time; */
    /* } */
  }

  return 0;
}
