#include "ft_ls.h"

char *name_set = " $'\"[]=()!";

OwnerCache *owner_cache = NULL;
GroupCache *group_cache = NULL;

enum SortType sort_type = sort_name;
enum Format format = many_per_line;

static char *VALID_OPTIONS[][4] = {
    {"-a", "--all", "do not ignore entries starting with .", NULL},
    {"-A", "--almost_all", "do not list implied . and ..", NULL},
    {NULL, "--author", "with -l, print the author of each file", NULL},
    {NULL, "--color", "color the output", NULL},
    {"-d", "--directory", "list directories themselves, not their contents",
     NULL},
    {"-f", NULL, "do not sort, enable -aU, disable -ls --color", NULL},
    {"-g", NULL, "like -l, but do not list owner", NULL},
    {"-G", "--no-group", "in a long listing, don't print group names", NULL},
    /* {"-h", "--human-readable", */
    /*  "with -l and -s, print sizes like 1K 234M 2G etc.", NULL}, */
    {"-l", NULL, "use a long listing format", NULL},
    {"-N", "--literal", "print entry names without quoting", NULL},
    {"-o", NULL, "like -l, but do not list group information", NULL},
    {"-r", "--reverse", "reverse order while sorting", NULL},
    {"-R", "--recursive", "list subdirectories recursively", NULL},
    {"-S", NULL, "sort by file size, largest first", NULL},
    {"-t", NULL, "sort by time, newest first; see --time", NULL},
    {"-u", NULL,
     "with  -lt: sort by, and show, access time; with -l: show access time and "
     "sort by name; otherwise: sort by acces time, newest first",
     NULL},
    {"-U", NULL, "do not sort; list entries in directory order", NULL},
    /* {"-x", NULL, "list entries by lines instead of by columns", NULL}, */
    /* {"-1", NULL, "list one file per line", NULL}, */

    {NULL, NULL},
};

void output_buffering(char *arr, size_t *pos, size_t capacity, char *str) {
  size_t len = ft_strlen(str);

  while (len > 0) {
    size_t space_left = capacity - *pos - 1;

    if (len > space_left) {
      ft_strncpy(arr + *pos, str, space_left);
      *pos += space_left;
      arr[*pos] = '\0';
      write(1, arr, *pos);
      *pos = 0;
      str += space_left;
      len -= space_left;
    } else {
      ft_strcpy(arr + *pos, str);
      *pos += len;
      break;
    }
  }
}

void print_short_option_not_found(char option) {
  char *prefix = "ft_ls: invalid option -- '";
  char *suffix = "'\nTry 'ft_ls --help' for more information.\n";

  size_t prefix_len = ft_strlen(prefix);
  size_t option_len = 1;
  size_t suffix_len = ft_strlen(suffix);

  char *msg = malloc(sizeof(char) * (prefix_len + option_len + suffix_len + 1));

  if (msg == NULL)
    return;

  ft_strcpy(msg, prefix);
  ft_strcpy(msg + prefix_len, &option);
  ft_strcpy(msg + prefix_len + option_len, suffix);

  msg[prefix_len + option_len + suffix_len] = '\0';

  ft_putstr(2, msg);

  free(msg);
}

void print_long_option_not_found(char *option) {
  char *prefix = "ft_ls: unrecognized option '";
  char *suffix = "'\nTry 'ft_ls --help' for more information.\n";

  size_t prefix_len = ft_strlen(prefix);
  size_t option_len = ft_strlen(option);
  size_t suffix_len = ft_strlen(suffix);

  char *msg = malloc(sizeof(char) * (prefix_len + option_len + suffix_len + 1));

  if (msg == NULL)
    return;

  ft_strcpy(msg, prefix);
  ft_strcpy(msg + prefix_len, option);
  ft_strcpy(msg + prefix_len + option_len, suffix);

  msg[prefix_len + option_len + suffix_len] = '\0';

  ft_putstr(2, msg);

  free(msg);
}

void input_not_found(char *input) {
  char *prefix = "ft_ls: cannot access '";
  char *suffix = "': No such file or directory\n";

  size_t prefix_len = ft_strlen(prefix);
  size_t option_len = ft_strlen(input);
  size_t suffix_len = ft_strlen(suffix);

  char *msg = malloc(sizeof(char) * (prefix_len + option_len + suffix_len + 1));

  if (msg == NULL)
    return;

  ft_strcpy(msg, prefix);
  ft_strcpy(msg + prefix_len, input);
  ft_strcpy(msg + prefix_len + option_len, suffix);

  msg[prefix_len + option_len + suffix_len] = '\0';

  ft_putstr(2, msg);

  free(msg);
}

char set_option(char c, char *opt) {
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

      if (short_opt == tmp[1])
        return set_option(tmp[1], NULL);
    }
  }

  return 0;
}

FileInfo *create_file_info(char *name, struct stat *stat) {
  FileInfo *info = malloc(sizeof(FileInfo));

  info->name = name;
  info->fullname = NULL;
  info->stat = stat;

  if (!S_ISDIR(stat->st_mode) && !S_ISLNK(stat->st_mode) &&
      ((stat->st_mode & S_IEXEC) | (stat->st_mode & S_IXGRP) |
       (stat->st_mode & S_IXOTH)))
    info->filetype = executable;
  else if (S_ISLNK(stat->st_mode))
    info->filetype = symbolic_link;
  else if (S_ISDIR(stat->st_mode))
    info->filetype = directory;
  else if (S_ISREG(stat->st_mode))
    info->filetype = normal;

  return info;
}

int ft_is_in_set(char c, char *set) {
  for (int i = 0; set[i] != '\0'; i++) {
    if (c == set[i])
      return 1;
  }

  return 0;
}

char *ft_has_set(char *str, char *set) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (ft_is_in_set(str[i], set))
      return &str[i];
  }

  return NULL;
}

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

char *skip_dots(char *str) {
  int i = 0;

  while (str[i] != '\0' && str[i] == '.')
    i++;

  return &str[i];
}

int ft_is_special_char(char c) {
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
      (c >= '0' && c <= '9'))
    return 0;

  return 1;
}

int ft_strcmp_alphanum(char *s1, char *s2) {
  int i = 0, j = 0;

  while (s1[i] != '\0' && s2[j] != '\0') {
    char a = s1[i];
    char b = s2[j];

    int is_a_in = ft_is_special_char(a);
    int is_b_in = ft_is_special_char(b);

    if (is_a_in || is_b_in) {
      if (is_a_in)
        i++;

      if (is_b_in)
        j++;

      continue;
    } else if (a != b)
      return a - b;

    i++;
    j++;
  }

  char a = s1[i];
  char b = s2[j];

  if (a != b)
    return a - b;

  return 0;
}

int ft_strcmp_alphanum_case(char *s1, char *s2) {
  int i = 0, j = 0;

  while (s1[i] != '\0' && s2[j] != '\0') {
    char a = ft_to_lower(s1[i]);
    char b = ft_to_lower(s2[j]);

    int is_a_in = ft_is_special_char(a);
    int is_b_in = ft_is_special_char(b);

    if (is_a_in || is_b_in) {
      if (is_a_in)
        i++;

      if (is_b_in)
        j++;

      continue;
    } else if (a != b)
      return a - b;

    i++;
    j++;
  }

  char a = ft_to_lower(s1[i]);
  char b = ft_to_lower(s2[j]);

  if (a != b)
    return a - b;

  return 0;
}

int ft_strcoll_lowercase(char *s1, char *s2) {
  int ret = ft_strcmp_alphanum_case(s1, s2);

  if (ret != 0)
    return ret;

  /* size_t s1_len = ft_strlen(s1); */
  /* size_t s2_len = ft_strlen(s2); */
  /**/
  /* if (s1_len < s2_len) */
  /*   return 1; */
  /* else if (s1_len > s2_len) */
  /*   return -1; */

  ret = ft_strcmp_lowercase(s1, s2);

  if (ret != 0)
    return ret;

  ret = ft_strcmp_alphanum(s1, s2);

  if (ret != 0)
    return ret;

  ret = ft_strcmp(s1, s2);

  if (ret < 0)
    return 1;
  else if (ret > 0)
    return -1;

  return 0;
}

int file_info_cmp_by_name(FileInfo *a, FileInfo *b) {
  return ft_strcmp(a->name, b->name);
}

int file_info_cmp_by_size(FileInfo *a, FileInfo *b) {
  long ret = b->stat->st_size - a->stat->st_size;

  if (ret < 0)
    return -1;
  else if (ret > 0)
    return 1;

  return file_info_cmp_by_name(a, b);
}

int file_info_cmp_by_update_time(FileInfo *a, FileInfo *b) {
  int64_t s1_time_ns =
      (a->stat->st_mtim.tv_sec * 1000000000LL) + a->stat->st_mtim.tv_nsec;
  int64_t s2_time_ns =
      (b->stat->st_mtim.tv_sec * 1000000000LL) + b->stat->st_mtim.tv_nsec;

  if (s1_time_ns < s2_time_ns)
    return 1;
  else if (s1_time_ns > s2_time_ns)
    return -1;

  return file_info_cmp_by_name(a, b);
}

int file_info_cmp_by_access_time(FileInfo *a, FileInfo *b) {
  int64_t s1_time_ns =
      (a->stat->st_atim.tv_sec * 1000000000LL) + a->stat->st_atim.tv_nsec;
  int64_t s2_time_ns =
      (b->stat->st_atim.tv_sec * 1000000000LL) + b->stat->st_atim.tv_nsec;

  if (s1_time_ns < s2_time_ns)
    return 1;
  else if (s1_time_ns > s2_time_ns)
    return -1;

  return file_info_cmp_by_name(a, b);
}

void sort_inputs(void) {
  if (sort_type == sort_update_time)
    ft_list_sort(&all.list, (CompareFunc)&file_info_cmp_by_update_time);
  else if (sort_type == sort_access_time)
    ft_list_sort(&all.list, (CompareFunc)&file_info_cmp_by_access_time);
  else if (sort_type == sort_name)
    ft_list_sort(&all.list, (CompareFunc)&file_info_cmp_by_name);
  else if (sort_type == sort_size)
    ft_list_sort(&all.list, (CompareFunc)&file_info_cmp_by_size);

  if (sort_reverse)
    ft_list_reverse(&all.list);
}

FileInfo **input_list_to_table(Input input) {
  FileInfo **tab = malloc(sizeof(FileInfo *) * (input.size + 1));
  List *list = input.list;
  int i = 0;

  while (list != NULL) {
    tab[i] = list->data;

    i++;
    list = list->next;
  }

  tab[i] = NULL;

  return tab;
}

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

void print_dir_name(FileInfo *file_info, int size) {
  print_file_name(file_info, file_info->filetype);

  while (size > 0) {
    ft_putchar(1, ' ');

    size--;
  }
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

char *get_owner_name(uid_t uid) {
  OwnerCache *current = owner_cache;

  while (current != NULL) {
    if (current->uid == uid)
      return current->owner_name;
    current = current->next;
  }

  struct passwd *owner = getpwuid(uid);
  if (!owner)
    return NULL;

  OwnerCache *new_cache = malloc(sizeof(OwnerCache));
  new_cache->uid = uid;
  new_cache->owner_name = ft_strdup(owner->pw_name);
  new_cache->next = owner_cache;
  owner_cache = new_cache;

  return new_cache->owner_name;
}

char *get_group_name(gid_t gid) {
  GroupCache *current = group_cache;

  while (current != NULL) {
    if (current->gid == gid)
      return current->group_name;
    current = current->next;
  }

  struct group *grp = getgrgid(gid);

  if (!grp)
    return NULL;

  GroupCache *new_cache = malloc(sizeof(GroupCache));
  new_cache->gid = gid;
  new_cache->group_name = ft_strdup(grp->gr_name);
  new_cache->next = group_cache;
  group_cache = new_cache;

  return new_cache->group_name;
}

void update_column_info(FileInfo *info, ColumnInfo *column_info) {
  column_info->block_size += info->stat->st_blocks;

  ssize_t ret = getxattr(info->fullname ? info->fullname : info->name,
                         "system.posix_acl_access", NULL, 0);

  if (ret >= 0) {
    column_info->print_acl = true;
    info->print_acl = true;
  } else
    info->print_acl = false;

  if (print_owner) {
    char *owner = get_owner_name(info->stat->st_uid);

    if (owner) {
      info->owner_name = owner;
      size_t owner_len = ft_strlen(owner);

      if (owner_len > column_info->owner_width)
        column_info->owner_width = owner_len;
    }
  }

  if (print_group) {
    char *group_name = get_group_name(info->stat->st_gid);

    if (group_name) {
      info->group_name = group_name;
      size_t group_len = ft_strlen(group_name);

      if (group_len > column_info->group_width)
        column_info->group_width = group_len;
    }
  }

  size_t len = ft_unsigned_number_len(info->stat->st_nlink);

  if (len > column_info->nlink_width)
    column_info->nlink_width = len;

  size_t size_len = ft_signed_number_len(info->stat->st_size);

  if (size_len > column_info->size_width)
    column_info->size_width = size_len;

  char *has_set = ft_has_set(info->name, name_set);

  if (has_set != NULL) {
    if (!column_info->print_quote)
      column_info->print_quote = true;

    info->print_quote = true;

    if (has_set[0] == '\'')
      info->has_single_quote = true;
    else
      info->has_single_quote = false;

  } else
    info->print_quote = false;
}

void calc_long_format(Input *input) {
  List *list = input->list;
  ColumnInfo *column_info = &input->column_info;

  while (list != NULL) {
    FileInfo *file_info = list->data;

    update_column_info(file_info, column_info);

    list = list->next;
  }
}

char *get_permission(bool condition, char *value) {
  return condition ? value : "-";
}

char *get_time(FileInfo *file_info) {
  time_t now, file_time;

  if (print_access_time) {
    file_time = file_info->stat->st_atim.tv_sec +
                (file_info->stat->st_atim.tv_nsec / 1000000000LL);
  } else {
    file_time = file_info->stat->st_mtim.tv_sec +
                (file_info->stat->st_mtim.tv_nsec / 1000000000LL);
  }

  char *time_str = ctime(&file_time);
  char *ret = NULL;

  double six_months_in_seconds = 182.5 * 24 * 60 * 60;

  time(&now);

  if (file_time > now - six_months_in_seconds) {
    ret = ft_substr(time_str, ft_index_of(time_str, ' ') + 1,
                    ft_last_index_of(time_str, ':') - 1);
  } else {
    char *year = ft_substr(time_str, ft_last_index_of(time_str, ' '),
                           ft_strlen(time_str) - 2);

    char *tmp = ft_substr(time_str, ft_index_of(time_str, ' ') + 1,
                          ft_index_of(time_str, ':') - 3);

    ret = ft_strjoin(tmp, year);
  }

  return ret;
}

void out_long_format(Input *input) {
  List *list = input->list;
  ColumnInfo column_info = input->column_info;
  size_t capacity = 256;
  char out[capacity];

  while (list != NULL) {
    size_t pos = 0;
    FileInfo *file_info = list->data;

    char temp[2] = {filetype_letter[file_info->filetype], '\0'};

    output_buffering(out, &pos, capacity, temp);

    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IRUSR, "r"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IWUSR, "w"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IEXEC, "x"));

    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IRGRP, "r"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IWGRP, "w"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IXGRP, "x"));

    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IROTH, "r"));
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IWOTH, "w"));

    if ((file_info->stat->st_mode & S_ISVTX))
      output_buffering(out, &pos, capacity,
                       get_permission(file_info->stat->st_mode & S_IXOTH, "t"));
    else
      output_buffering(out, &pos, capacity,
                       get_permission(file_info->stat->st_mode & S_IXOTH, "x"));

    if (input->column_info.print_acl) {
      if (file_info->print_acl)
        output_buffering(out, &pos, capacity, "+");
      else
        output_buffering(out, &pos, capacity, " ");
    }

    output_buffering(out, &pos, capacity, " ");

    int size = column_info.nlink_width -
               ft_unsigned_number_len(file_info->stat->st_nlink);

    while (size > 0) {
      output_buffering(out, &pos, capacity, " ");

      size--;
    }

    char *number = ft_unsigned_num_to_str(file_info->stat->st_nlink);
    output_buffering(out, &pos, capacity, number);
    free(number);

    output_buffering(out, &pos, capacity, " ");

    if (print_owner) {
      output_buffering(out, &pos, capacity, file_info->owner_name);

      size = column_info.owner_width - ft_strlen(file_info->owner_name);

      while (size > 0) {
        output_buffering(out, &pos, capacity, " ");

        size--;
      }

      output_buffering(out, &pos, capacity, " ");
    }

    if (print_group) {
      output_buffering(out, &pos, capacity, file_info->group_name);

      size = column_info.group_width - ft_strlen(file_info->group_name);

      while (size > 0) {
        output_buffering(out, &pos, capacity, " ");

        size--;
      }

      output_buffering(out, &pos, capacity, " ");
    }

    size =
        column_info.size_width - ft_signed_number_len(file_info->stat->st_size);

    while (size > 0) {
      output_buffering(out, &pos, capacity, " ");

      size--;
    }

    number = ft_signed_num_to_str(file_info->stat->st_size);
    output_buffering(out, &pos, capacity, number);
    free(number);

    output_buffering(out, &pos, capacity, " ");

    char *time = get_time(file_info);

    if (time != NULL) {
      size = 12 - ft_strlen(time);

      while (size > 0) {
        output_buffering(out, &pos, capacity, " ");

        size--;
      }

      output_buffering(out, &pos, capacity, time);
      free(time);
    }

    output_buffering(out, &pos, capacity, " ");

    if (print_quotes && column_info.print_quote && !file_info->print_quote)
      output_buffering(out, &pos, capacity, " ");

    if (print_with_color) {
      if ((file_info->stat->st_mode & S_ISVTX)) {
        output_buffering(out, &pos, capacity, BLACK);
        output_buffering(out, &pos, capacity, STICKY_BIT_COLOR);
      } else
        output_buffering(out, &pos, capacity,
                         filetype_color[file_info->filetype]);
    }

    if (print_quotes && file_info->print_quote) {
      if (file_info->has_single_quote)
        output_buffering(out, &pos, capacity, "\"");
      else
        output_buffering(out, &pos, capacity, "'");

      output_buffering(out, &pos, capacity, file_info->name);

      if (file_info->has_single_quote)
        output_buffering(out, &pos, capacity, "\"");
      else
        output_buffering(out, &pos, capacity, "'");
    } else
      output_buffering(out, &pos, capacity, file_info->name);

    if (print_with_color) {
      if ((file_info->stat->st_mode & S_ISVTX))
        output_buffering(out, &pos, capacity, RESET_BACK);

      output_buffering(out, &pos, capacity, WHITE);
    }

    while (size > 0) {
      output_buffering(out, &pos, capacity, " ");

      size--;
    }

    if (file_info->filetype == symbolic_link) {
      output_buffering(out, &pos, capacity, " -> ");

      char l_name[256];
      ssize_t size = readlink(file_info->fullname, l_name, sizeof(l_name) - 1);

      if (size != -1) {
        l_name[size] = '\0';

        output_buffering(out, &pos, capacity, l_name);
      }
    }

    output_buffering(out, &pos, capacity, "\n");

    out[pos] = '\0';

    ft_putstr(1, out);

    list = list->next;
  }
}

void print_block_size(unsigned long size) {
  ft_putstr(1, "total ");
  ft_putnbr(1, size / 2);
  ft_putstr(1, "\n");
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

void separate_input(void) {
  List *list = all.list;

  while (list != NULL) {
    List *tmp = list;
    FileInfo *file_info = list->data;

    if (file_info->filetype == normal || file_info->filetype == symbolic_link ||
        file_info->filetype == executable) {
      ft_list_push_back(&files.list, file_info);
      files.size++;
    } else if (file_info->filetype == directory) {
      ft_list_push_back(&dirs.list, file_info);
      dirs.size++;
    }

    list = list->next;
    free(tmp);
  }

  all.list = NULL;
}

void list_print(void *data) {
  FileInfo *info = data;

  printf("'%s' -> { fullname: %s, size: %ld, last_update: %lu, "
         "type: %s }\n",
         info->name, info->fullname, info->stat->st_size,
         info->stat->st_mtim.tv_sec,
         info->filetype == normal || info->filetype == symbolic_link ? "FILE"
                                                                     : "DIR");
}

void process_dir_content(FileInfo *file_info, int depth) {
  char *name = file_info
                   ? file_info->fullname ? file_info->fullname : file_info->name
                   : ".";

  DIR *o_dir = opendir(name);

  if (!o_dir) {
    ft_putstr(1, program_name);
    ft_putstr(1, ": cannot open directory '");
    ft_putstr(1, name);
    ft_putstr(1, "': ");
    ft_putstr(1, strerror(errno));
    ft_putchar(1, '\n');

    return;
  }

  if (recursive) {
    if (depth > 0)
      ft_putchar(1, '\n');
    depth++;

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

  size_t name_len = ft_strlen(name);
  char full_path[name_len + 256];

  ft_strcpy(full_path, name);

  if (full_path[name_len - 1] != '/')
    full_path[name_len++] = '/';

  struct dirent *content;
  Input input;

  input.list = NULL;
  input.size = 0;

  init_column_info(&input.column_info);

  while ((content = readdir(o_dir))) {
    if (ignore_hidden_files && content->d_name[0] == '.')
      continue;

    if (ignore_dots && (ft_strcmp(".", content->d_name) == 0 ||
                        ft_strcmp("..", content->d_name) == 0))
      continue;

    ft_strcpy(full_path + name_len, content->d_name);

    struct stat *stat = malloc(sizeof(struct stat));

    if (lstat(full_path, stat) == -1) {
      perror("lstat");
      continue;
    }

    FileInfo *info = create_file_info(ft_strdup(content->d_name), stat);
    info->fullname = ft_strdup(full_path);
    update_column_info(info, &input.column_info);

    ft_list_push_back(&input.list, info);
    input.size++;
  }

  if (sort_type == sort_update_time)
    ft_list_sort(&input.list, (CompareFunc)&file_info_cmp_by_update_time);
  else if (sort_type == sort_access_time)
    ft_list_sort(&input.list, (CompareFunc)&file_info_cmp_by_access_time);
  else if (sort_type == sort_name)
    ft_list_sort(&input.list, (CompareFunc)&file_info_cmp_by_name);
  else if (sort_type == sort_size)
    ft_list_sort(&input.list, (CompareFunc)&file_info_cmp_by_size);

  if (sort_reverse)
    ft_list_reverse(&input.list);

  if (format == long_format) {
    print_block_size(input.column_info.block_size);
    out_long_format(&input);
  } else if (format == many_per_line) {
    FileInfo **list = input_list_to_table(input);

    calc_many_per_line_format(&input, list);
    out_column_format(&input, list);

    clear_column_info(&input.column_info);
    free(list);
  }

  if (recursive) {
    List *list = input.list;

    while (list != NULL) {
      FileInfo *file_info = list->data;

      if (!ignore_hidden_files) {
        if (ft_strcmp(".", file_info->name) == 0 ||
            ft_strcmp("..", file_info->name) == 0) {
          list = list->next;

          continue;
        }
      }

      if (file_info->filetype == directory)
        process_dir_content(file_info, depth);

      list = list->next;
    }
  }

  ft_list_clear(input.list, (ClearFunc)&clear_file_info);
  closedir(o_dir);
}

void process_inputs(void) {
  if (all.size == 0 && all.error)
    exit_status = 2;
  else if (immediate_dirs)
    print_out_format(all);
  else if (all.size == 0)
    process_dir_content(NULL, 0);
  else {
    separate_input();

    if (files.size > 0) {
      print_out_format(files);

      if (dirs.size > 0 || all.error)
        ft_putchar(1, '\n');
    }

    if (dirs.size > 0) {
      List *list = dirs.list;

      while (list != NULL) {
        FileInfo *info = list->data;

        if (dirs.size > 1 || files.size > 0) {
          char *has_set = ft_has_set(info->name, name_set);
          bool print_quote = print_quotes && has_set != NULL;

          if (print_quote) {
            if (has_set[0] == '\'')
              ft_putchar(1, '"');
            else
              ft_putchar(1, '\'');
          }

          ft_putstr(1, info->name);

          if (print_quote) {
            if (has_set[0] == '\'')
              ft_putchar(1, '"');
            else
              ft_putchar(1, '\'');
          }

          ft_putstr(1, ":\n");
        }

        process_dir_content(info, 0);

        if (dirs.size > 1 && list->next != NULL)
          ft_putchar(1, '\n');

        list = list->next;
      }
    } else if (files.size == 0) {
      process_dir_content(NULL, 0);
    }
  }
}

int main(int argc, char *argv[]) {
  init_inputs();

  /* program_name = argv[0]; */
  program_name = "/bin/ls";

  if (parse_args(argc, argv) == 0) {
    sort_inputs();
    process_inputs();
  }

  clear_inputs();

  return exit_status;
}
