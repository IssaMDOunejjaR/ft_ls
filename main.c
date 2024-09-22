#include "headers/ft_lib.h"
#include "headers/ft_ls.h"
#include "libcft/ds/list/ft_list.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

RessourceType check_ressource_type(char *path) {
  struct stat lstats;

  if (lstat(path, &lstats) == -1) {
    perror("lstat");

    return _NONE;
  }

  if (S_ISLNK(lstats.st_mode))
    return _SYMLIK;
  else if (S_ISDIR(lstats.st_mode))
    return _DIR;
  else if (S_ISREG(lstats.st_mode))
    return _FILE;

  return _NONE;
}

char *VALID_OPTIONS[][4] = {
    {"-a", "--all", "do not ignore entries starting with .", NULL},
    {NULL, "--color", "color the output", NULL},
    {"-d", "--directory", "list directories themselves, not their contents",
     NULL},
    {"-f", NULL, "do not sort, enable -aU, disable -ls --color", NULL},
    {"-g", NULL, "like -l, but do not list owner", NULL},
    {"-G", "--no-group", "in a long listing, don't print group names", NULL},
    {"-l", NULL, "use a long listing format", NULL},
    {"-r", "--reverse", "reverse order while sorting", NULL},
    {"-R", "--recursive", "list subdirectories recursively", NULL},
    {"-t", NULL, "sort by time, newest first; see --time", NULL},
    {"-u", NULL,
     "with  -lt: sort by, and show, access time; with -l: show access time and "
     "sort by name; otherwise: sort by acces time, newest first",
     NULL},

    {NULL, NULL},
};

int str_cmp(void *s1, void *s2) { return ft_strcmp(s1, s2); }

void print(void *data) { printf(" - %s\n", (char *)data); }

void init_args(Args *args) {
  args->dirs.list = NULL;
  args->dirs.size = 0;

  args->files.list = NULL;
  args->files.size = 0;
  args->files.values.col_widths = NULL;
  args->files.values.terminal_width = 0;
  args->files.values.num_columns = 1;
  args->files.values.num_rows = 0;
  args->files.values.gap = 2;
  args->files.values.quote_space = 0;

  args->options.listing = false;
  args->options.hidden = false;
  args->options.sort = true;
  args->options.reverse = false;
  args->options.owner = true;
  args->options.group = true;
  args->options.directory = false;
  args->options.recursive = false;
  args->options.sort_by_time = false;
  args->options.color = false;
}

void clear_args(Args *args) {}

int is_valid_option() { return 0; }

void print_help() {
  ft_putendl("Usage: ft_ls [OPTION]... [FILE]...");
  ft_putendl(
      "List information about the FILEs (the current directory by default).");
  ft_putchar('\n');
  ft_putendl("Mandatory arguments to long options are mandatory for short "
             "options too.");

  int start_pad = 2;
  int middle_gap = 5;
  int left_pad = 0;

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
      ft_putchar(' ');

    if (VALID_OPTIONS[i][0] != NULL) {
      ft_putstr(VALID_OPTIONS[i][0]);
      ft_putstr(", ");
    }

    if (VALID_OPTIONS[i][1] != NULL)
      ft_putstr(VALID_OPTIONS[i][1]);

    size_t option_len = ft_strlen(VALID_OPTIONS[i][0]) +
                        ft_strlen(VALID_OPTIONS[i][1]) +
                        (VALID_OPTIONS[i][0] ? 2 : 0);

    for (int j = 0; j < (left_pad - option_len - start_pad) + middle_gap; j++)
      ft_putchar(' ');

    int max_paragraph_len = 70, count = 0;

    for (int j = 0; VALID_OPTIONS[i][2][j] != '\0'; j++) {
      ft_putchar(VALID_OPTIONS[i][2][j]);
      count++;

      if (count == max_paragraph_len) {
        count = 0;
        ft_putchar('\n');

        for (int k = 0; k < left_pad + middle_gap; k++)
          ft_putchar(' ');
      }
    }

    ft_putchar('\n');
  }
}

char set_option(Args *args, char c, char *opt) {
  if (opt != NULL) {
    if (ft_strcmp(opt, "--color") == 0) {
      args->options.color = true;
    } else
      return 0;

    return 1;
  } else if (c == 'a') {
    args->options.hidden = true;
  } else if (c == 'd') {
    args->options.directory = true;
  } else if (c == 'f') {
    args->options.sort = false;
    args->options.color = false;
    args->options.listing = false;
  } else if (c == 'g') {
    args->options.owner = false;
    args->options.listing = true;
  } else if (c == 'l') {
    args->options.listing = true;
  } else if (c == 'r') {
    args->options.reverse = true;
  } else if (c == 'R') {
    args->options.recursive = true;
  } else if (c == 't') {
    args->options.sort_by_time = true;
  } else if (c == 'u') {
  } else if (c == 'G') {
    args->options.group = false;
  } else {
    return 0;
  }

  return c;
}

int check_long_option(Args *args, char *str) {
  for (int i = 0; VALID_OPTIONS[i][0] != NULL || VALID_OPTIONS[i][1]; i++) {
    char *tmp = VALID_OPTIONS[i][1];

    if (tmp == NULL)
      continue;

    if (ft_strcmp(str, tmp) == 0) {
      if (VALID_OPTIONS[i][0] == NULL)
        return set_option(args, 0, tmp);

      return set_option(args, VALID_OPTIONS[i][0][1], NULL);
    }
  }

  return 0;
}

int check_short_option(Args *args, char c) {
  for (int i = 0; VALID_OPTIONS[i][0] != NULL || VALID_OPTIONS[i][1] != NULL;
       i++) {
    char *tmp = VALID_OPTIONS[i][0];

    if (tmp == NULL)
      continue;

    if (c == tmp[1])
      return set_option(args, tmp[1], NULL);
  }

  return 0;
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

  ft_putstr(msg);

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

  ft_putstr(msg);

  free(msg);
}

char *is_set(bool condition) { return condition ? "✓" : "x"; }

void print_options(Args *args) {
  printf("Options:\n");
  printf("  [%s] hidden\n", is_set(args->options.hidden));
  printf("  [%s] directory\n", is_set(args->options.directory));
  printf("  [%s] sort\n", is_set(args->options.sort));
  printf("  [%s] color\n", is_set(args->options.color));
  printf("  [%s] owner\n", is_set(args->options.owner));
  printf("  [%s] listing\n", is_set(args->options.listing));
  printf("  [%s] reverse\n", is_set(args->options.reverse));
  printf("  [%s] recursive\n", is_set(args->options.recursive));
  printf("  [%s] sort_by_time\n", is_set(args->options.sort_by_time));
  printf("  [%s] group\n", is_set(args->options.sort_by_time));
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

  ft_putstr(msg);

  free(msg);
}

typedef struct {
  bool is_directory;
  bool is_symlink;
  struct stat stats;
  struct stat lstats;
  char *owner;
  char *group;
  char *name;
} Info;

size_t list_size(void **lst) {
  size_t size = 0;

  if (lst == NULL)
    return size;

  for (int i = 0; lst[i] != NULL; i++)
    size++;

  return size;
}

void list_free(void **lst) {
  if (lst == NULL)
    return;

  for (int i = 0; lst[i] != NULL; i++)
    free(lst[i]);

  free(lst);
}

Info **list_append(Info **lst, Info *value) {
  Info **new_list = NULL;

  if (lst == NULL) {
    new_list = malloc(sizeof(Info *) * 2);

    new_list[0] = value;
    new_list[1] = NULL;

    return new_list;
  }

  new_list = malloc(sizeof(Info *) * (list_size((void **)lst) + 2));
  int i;

  for (i = 0; lst[i] != NULL; i++)
    new_list[i] = lst[i];

  new_list[i++] = value;
  new_list[i] = NULL;

  free(lst);

  return new_list;
}

typedef bool (*CompareFunc)(void *a, void *b);

char *skip_dots(char *str) {
  int i = 0;

  while (str[i] == '.')
    i++;

  if (str[i] == '\0')
    return str;

  return &str[i];
}

bool info_cmp(Info *a, Info *b) {
  char *tmp_a = ft_to_lowercase(a->name);
  char *tmp_b = ft_to_lowercase(b->name);

  if (ft_strcmp(tmp_a, tmp_b) > 0) {
    free(tmp_a);
    free(tmp_b);

    return true;
  }

  free(tmp_a);
  free(tmp_b);

  return false;
}

void print_spaces(char *str, int size) {
  ft_putstr(str);

  while (size > 0) {
    ft_putchar(' ');

    size--;
  }
}

void print_number_spaces(int number, int size) {
  while (size > 0) {
    ft_putchar(' ');

    size--;
  }

  ft_putnbr(number);
}

/* void listing_print(Args *args, char *parent_dir) { */
/*   DIR *o_dir = opendir(parent_dir); */
/*   struct dirent *dir; */
/*   Info **infos = NULL; */
/*   int gap = 1; */
/*   int quote_space = 0; */
/*   int link_size_column = 0; */
/*   int owner_column = 0; */
/*   int group_column = 0; */
/*   int size_column = 0; */
/*   int block_size = 0; */
/**/
/*   while ((dir = readdir(o_dir))) { */
/*     if (!args->options.hidden && dir->d_name[0] == '.') */
/*       continue; */
/**/
/*     Info *info = malloc(sizeof(Info)); */
/**/
/*     char *tmp = ft_strjoin(parent_dir, "/"); */
/*     char *path = ft_strjoin(tmp, dir->d_name); */
/**/
/*     if (stat(path, &info->stats) == -1) { */
/*       perror("stat"); */
/**/
/*       return; */
/*     } */
/**/
/*     if (lstat(path, &info->lstats) == -1) { */
/*       perror("lstat"); */
/**/
/*       return; */
/*     } */
/**/
/*     if (S_ISLNK(info->lstats.st_mode)) { */
/*       info->is_symlink = true; */
/**/
/*       info->owner = ft_strdup(getpwuid(info->lstats.st_uid)->pw_name); */
/*       info->group = ft_strdup(getgrgid(info->lstats.st_gid)->gr_name); */
/**/
/*       size_t owner_len = ft_strlen(info->owner); */
/**/
/*       if (owner_len > owner_column) */
/*         owner_column = owner_len; */
/**/
/*       size_t group_len = ft_strlen(info->group); */
/**/
/*       if (group_len > group_column) */
/*         group_column = group_len; */
/**/
/*       size_t len = ft_number_len(info->lstats.st_nlink); */
/**/
/*       if (len > link_size_column) */
/*         link_size_column = len; */
/**/
/*       size_t size_len = ft_number_len(info->lstats.st_size); */
/**/
/*       if (size_len > size_column) */
/*         size_column = size_len; */
/*     } else { */
/*       block_size += info->stats.st_blocks; */
/*       info->is_symlink = false; */
/*       if (S_ISDIR(info->stats.st_mode)) { */
/*         info->is_directory = true; */
/*       } else if (S_ISREG(info->stats.st_mode)) { */
/*         info->is_directory = false; */
/*       } */
/**/
/*       info->owner = ft_strdup(getpwuid(info->stats.st_uid)->pw_name); */
/*       info->group = ft_strdup(getgrgid(info->stats.st_gid)->gr_name); */
/**/
/*       size_t owner_len = ft_strlen(info->owner); */
/**/
/*       if (owner_len > owner_column) */
/*         owner_column = owner_len; */
/**/
/*       size_t group_len = ft_strlen(info->group); */
/**/
/*       if (group_len > group_column) */
/*         group_column = group_len; */
/**/
/*       size_t len = ft_number_len(info->stats.st_nlink); */
/**/
/*       if (len > link_size_column) */
/*         link_size_column = len; */
/**/
/*       size_t size_len = ft_number_len(info->stats.st_size); */
/**/
/*       if (size_len > size_column) */
/*         size_column = size_len; */
/*     } */
/**/
/*     info->name = dir->d_name; */
/**/
/*     if (ft_strchr(dir->d_name, ' ') != NULL) */
/*       quote_space = 1; */
/**/
/*     infos = list_append(infos, info); */
/*   } */
/**/
/*   ft_putstr("total "); */
/*   ft_putnbr(block_size); */
/*   ft_putchar('\n'); */
/*   if (infos) { */
/*     infos = (Info **)ft_bubble_sort((void **)infos, (CompareFunc)&info_cmp);
 */
/**/
/*     for (int i = 0; infos[i] != NULL; i++) { */
/*       if (infos[i]->is_symlink) { */
/*         ft_putstr("l"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IRUSR) ? "r" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IWUSR) ? "w" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IEXEC) ? "x" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IRGRP) ? "r" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IWGRP) ? "w" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IXGRP) ? "x" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IROTH) ? "r" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IWOTH) ? "w" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IXOTH) ? "x" : "-"); */
/**/
/*         ft_putchar(' '); */
/**/
/*         print_number_spaces(infos[i]->lstats.st_nlink, */
/*                             link_size_column - */
/*                                 ft_number_len(infos[i]->lstats.st_nlink)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         if (args->options.owner) { */
/*           print_spaces(infos[i]->owner, */
/*                        owner_column - ft_strlen(infos[i]->owner)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         if (args->options.group) { */
/*           print_spaces(infos[i]->group, */
/*                        group_column - ft_strlen(infos[i]->group)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         print_number_spaces(infos[i]->lstats.st_size, */
/*                             size_column - */
/*                                 ft_number_len(infos[i]->lstats.st_size)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec); */
/*         char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1, */
/*                               ft_last_index_of(time, ':') - 1); */
/*         if (tmp != NULL) { */
/*           ft_putstr(tmp); */
/*           free(tmp); */
/*         } */
/**/
/*         ft_putchar(' '); */
/*         if (quote_space && ft_strchr(infos[i]->name, ' ') == NULL) */
/*           ft_putchar(' '); */
/**/
/*         print_dir_name(args, infos[i]->name, 0, true, false, false); */
/*         ft_putstr(" -> "); */
/**/
/*         char l_name[1000]; */
/*         ssize_t size = readlink(infos[i]->name, l_name, sizeof(l_name) - 1);
 */
/*         l_name[size] = '\0'; */
/**/
/*         if (size == -1) { */
/*           perror("readlink"); */
/**/
/*           return; */
/*         } */
/**/
/*         ft_putendl(l_name); */
/*       } else { */
/*         if (infos[i]->is_directory) */
/*           ft_putstr("d"); */
/*         else */
/*           ft_putstr("-"); */
/**/
/*         ft_putstr((infos[i]->stats.st_mode & S_IRUSR) ? "r" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IWUSR) ? "w" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IEXEC) ? "x" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IRGRP) ? "r" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IWGRP) ? "w" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IXGRP) ? "x" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IROTH) ? "r" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IWOTH) ? "w" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IXOTH) ? "x" : "-"); */
/**/
/*         ft_putchar(' '); */
/**/
/*         print_number_spaces(infos[i]->stats.st_nlink, */
/*                             link_size_column - */
/*                                 ft_number_len(infos[i]->stats.st_nlink)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         if (args->options.owner) { */
/*           print_spaces(infos[i]->owner, */
/*                        owner_column - ft_strlen(infos[i]->owner)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         if (args->options.group) { */
/*           print_spaces(infos[i]->group, */
/*                        group_column - ft_strlen(infos[i]->group)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         print_number_spaces(infos[i]->stats.st_size, */
/*                             size_column - */
/*                                 ft_number_len(infos[i]->stats.st_size)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec); */
/*         char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1, */
/*                               ft_last_index_of(time, ':') - 1); */
/*         if (tmp != NULL) { */
/*           ft_putstr(tmp); */
/*           free(tmp); */
/*         } */
/**/
/*         ft_putchar(' '); */
/*         if (quote_space && ft_strchr(infos[i]->name, ' ') == NULL) */
/*           ft_putchar(' '); */
/**/
/*         int is_executable = infos[i]->stats.st_mode & S_IEXEC | */
/*                             infos[i]->stats.st_mode & S_IXGRP | */
/*                             infos[i]->stats.st_mode & S_IXOTH; */
/**/
/*         print_dir_name(args, infos[i]->name, 0, false,
 * infos[i]->is_directory, */
/*                        is_executable); */
/*         ft_putchar('\n'); */
/*       } */
/**/
/*       free(infos[i]->owner); */
/*       free(infos[i]->group); */
/*     } */
/*   } */
/**/
/*   list_free((void **)infos); */
/*   closedir(o_dir); */
/* } */

int parse_args(Args *args, int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (ft_strcmp(argv[i], "--help") == 0) {
      print_help();

      clear_args(args);

      return 0;
    } else if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        if (check_long_option(args, argv[i]) == 0) {
          print_long_option_not_found(argv[i]);

          return 2;
        }
      } else {
        for (int j = 1; argv[i][j] != '\0'; j++) {
          if (check_short_option(args, argv[i][j]) == 0) {
            print_short_option_not_found(argv[i][j]);

            return 2;
          }
        }
      }
    } else {
      RessourceType rs = check_ressource_type(argv[i]);

      if (rs == _DIR) {
        if (args->options.sort)
          ft_sorted_list_insert(&args->dirs.list, ft_strdup(argv[i]), &str_cmp);
        else
          ft_list_push_back(&args->dirs.list, ft_strdup(argv[i]));
        args->dirs.size++;
      } else if (rs == _FILE || rs == _SYMLIK) {
        if (args->options.sort)
          ft_sorted_list_insert(&args->files.list, ft_strdup(argv[i]),
                                &str_cmp);
        else
          ft_list_push_back(&args->files.list, ft_strdup(argv[i]));
        args->files.size++;
      } else
        input_not_found(argv[i]);
    }
  }

  return 0;
}

void calc_columns_values(ColumnValues *values, char **list, size_t list_size) {
  size_t num_columns = 1, num_rows = list_size;
  size_t *col_widths = NULL, *tmp = NULL;

  while (1) {
    num_rows = (list_size + num_columns - 1) / num_columns;

    if (col_widths != NULL) {
      if (num_columns > 1) {
        if (tmp != NULL)
          free(tmp);
        size_t size = sizeof(size_t) * (num_columns - 1);
        tmp = malloc(size);
        if (tmp == NULL)
          return;
        memcpy(tmp, col_widths, size);
      }
      free(col_widths);
    }

    col_widths = malloc(sizeof(size_t) * num_columns);

    if (!col_widths)
      return;

    size_t total_width = values->quote_space;

    for (size_t col = 0; col < num_columns; col++) {
      size_t max_len = 0;

      for (size_t row = 0; row < num_rows; row++) {
        size_t index = row + col * num_rows;

        if (index < list_size) {
          size_t len = ft_strlen(list[index]);

          if (ft_strchr(list[index], ' ') != NULL)
            len++;

          if (len > max_len) {
            max_len = len;
          }
        }
      }

      col_widths[col] = max_len;
      total_width += max_len;

      if (col < num_columns - 1) {
        total_width += values->gap;
      }
    }

    if (total_width >= values->terminal_width) {
      if (num_columns > 1) {
        free(col_widths);
        col_widths = tmp;
        tmp = NULL;
        num_columns--;
        num_rows = (list_size + num_columns - 1) / num_columns;
      }

      break;
    } else if (num_columns == list_size) {
      free(tmp);

      break;
    }

    num_columns++;
  }

  values->num_columns = num_columns;
  values->num_rows = num_rows;
  values->col_widths = col_widths;
}

void print_columns(Args *args, ColumnValues *values, char **list,
                   size_t list_size, char *parent_dir) {
  for (int row = 0; row < values->num_rows; row++) {
    for (int col = 0; col < values->num_columns; col++) {
      int index = row + col * values->num_rows;

      if (index < list_size) {
        char *is_spaced_name = strchr(list[index], ' ');

        if (col == 0 && values->quote_space == 1 && is_spaced_name == NULL)
          ft_putchar(' ');

        int spaces = 0;
        int next_word = row + (col + 1) * values->num_rows;

        if (col < values->num_columns - 1) {
          spaces = values->col_widths[col] - strlen(list[index]) + values->gap;

          if (next_word < list_size && strchr(list[next_word], ' ') != NULL) {
            spaces--;
          }
        }

        if (is_spaced_name != NULL)
          spaces--;

        struct stat lstats, stats;
        char *tmp = ft_strjoin(parent_dir, "/");
        char *path = ft_strjoin(tmp, list[index]);

        if (lstat(path, &lstats) == -1) {
          perror("lstat");
          return;
        }

        if (stat(path, &stats) == -1) {
          perror("stat");
          return;
        }

        free(tmp);
        free(path);

        bool is_symlink = false;
        bool is_executable = false;
        bool is_directory = false;

        if (S_ISLNK(lstats.st_mode)) {
          is_symlink = true;
          is_executable = lstats.st_mode & S_IEXEC | lstats.st_mode & S_IXGRP |
                          lstats.st_mode & S_IXOTH;
        } else {
          if (S_ISDIR(stats.st_mode)) {
            is_directory = true;
          } else if (S_ISREG(stats.st_mode)) {
            is_directory = false;
          }
          is_executable = lstats.st_mode & S_IEXEC | lstats.st_mode & S_IXGRP |
                          lstats.st_mode & S_IXOTH;
        }

        print_dir_name(args, list[index], spaces, is_symlink, is_directory,
                       is_executable);
      }
    }

    ft_putchar('\n');
  }

  free(values->col_widths);
}

unsigned long get_terminal_width() {
  struct winsize w;

  if (ioctl(0, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");

    return 0;
  }

  return w.ws_col;
}

void init_columns_values(ColumnValues *values) {
  values->gap = 2;
  values->num_rows = 0;
  values->num_columns = 1;
  values->col_widths = NULL;
  values->quote_space = 0;
  values->info = NULL;
  values->block_size = 0;
  values->size_column = 0;
  values->group_column = 0;
  values->owner_column = 0;
  values->link_size_column = 0;
  values->terminal_width = get_terminal_width();
}

void column_printing(Args *args, ColumnValues *values, char **list,
                     size_t list_size, char *parent_dir) {
  init_columns_values(values);

  calc_columns_values(values, list, list_size);

  print_columns(args, values, list, list_size, parent_dir);
}

/* void list_printing(Args *args, char **list, size_t list_size, */
/*                    char *parent_dir) { */
/*   DIR *o_dir = opendir(parent_dir); */
/*   struct dirent *dir; */
/*   Info **infos = NULL; */
/*   int gap = 1; */
/*   int quote_space = 0; */
/*   int link_size_column = 0; */
/*   int owner_column = 0; */
/*   int group_column = 0; */
/*   int size_column = 0; */
/*   int block_size = 0; */
/**/
/*   while ((dir = readdir(o_dir))) { */
/*     if (!args->options.hidden && dir->d_name[0] == '.') */
/*       continue; */
/**/
/*     Info *info = malloc(sizeof(Info)); */
/**/
/*     char *tmp = ft_strjoin(parent_dir, "/"); */
/*     char *path = ft_strjoin(tmp, dir->d_name); */
/**/
/*     if (stat(path, &info->stats) == -1) { */
/*       perror("stat"); */
/**/
/*       return; */
/*     } */
/**/
/*     if (lstat(path, &info->lstats) == -1) { */
/*       perror("lstat"); */
/**/
/*       return; */
/*     } */
/**/
/*     if (S_ISLNK(info->lstats.st_mode)) { */
/*       info->is_symlink = true; */
/**/
/*       info->owner = ft_strdup(getpwuid(info->lstats.st_uid)->pw_name); */
/*       info->group = ft_strdup(getgrgid(info->lstats.st_gid)->gr_name); */
/**/
/*       size_t owner_len = ft_strlen(info->owner); */
/**/
/*       if (owner_len > owner_column) */
/*         owner_column = owner_len; */
/**/
/*       size_t group_len = ft_strlen(info->group); */
/**/
/*       if (group_len > group_column) */
/*         group_column = group_len; */
/**/
/*       size_t len = ft_number_len(info->lstats.st_nlink); */
/**/
/*       if (len > link_size_column) */
/*         link_size_column = len; */
/**/
/*       size_t size_len = ft_number_len(info->lstats.st_size); */
/**/
/*       if (size_len > size_column) */
/*         size_column = size_len; */
/*     } else { */
/*       block_size += info->stats.st_blocks; */
/*       info->is_symlink = false; */
/*       if (S_ISDIR(info->stats.st_mode)) { */
/*         info->is_directory = true; */
/*       } else if (S_ISREG(info->stats.st_mode)) { */
/*         info->is_directory = false; */
/*       } */
/**/
/*       info->owner = ft_strdup(getpwuid(info->stats.st_uid)->pw_name); */
/*       info->group = ft_strdup(getgrgid(info->stats.st_gid)->gr_name); */
/**/
/*       size_t owner_len = ft_strlen(info->owner); */
/**/
/*       if (owner_len > owner_column) */
/*         owner_column = owner_len; */
/**/
/*       size_t group_len = ft_strlen(info->group); */
/**/
/*       if (group_len > group_column) */
/*         group_column = group_len; */
/**/
/*       size_t len = ft_number_len(info->stats.st_nlink); */
/**/
/*       if (len > link_size_column) */
/*         link_size_column = len; */
/**/
/*       size_t size_len = ft_number_len(info->stats.st_size); */
/**/
/*       if (size_len > size_column) */
/*         size_column = size_len; */
/*     } */
/**/
/*     info->name = dir->d_name; */
/**/
/*     if (ft_strchr(dir->d_name, ' ') != NULL) */
/*       quote_space = 1; */
/**/
/*     infos = list_append(infos, info); */
/*   } */
/**/
/*   ft_putstr("total "); */
/*   ft_putnbr(block_size); */
/*   ft_putchar('\n'); */
/*   if (infos) { */
/*     infos = (Info **)ft_bubble_sort((void **)infos, (CompareFunc)&info_cmp);
 */
/**/
/*     for (int i = 0; infos[i] != NULL; i++) { */
/*       if (infos[i]->is_symlink) { */
/*         ft_putstr("l"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IRUSR) ? "r" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IWUSR) ? "w" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IEXEC) ? "x" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IRGRP) ? "r" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IWGRP) ? "w" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IXGRP) ? "x" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IROTH) ? "r" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IWOTH) ? "w" : "-"); */
/*         ft_putstr((infos[i]->lstats.st_mode & S_IXOTH) ? "x" : "-"); */
/**/
/*         ft_putchar(' '); */
/**/
/*         print_number_spaces(infos[i]->lstats.st_nlink, */
/*                             link_size_column - */
/*                                 ft_number_len(infos[i]->lstats.st_nlink)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         if (args->options.owner) { */
/*           print_spaces(infos[i]->owner, */
/*                        owner_column - ft_strlen(infos[i]->owner)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         if (args->options.group) { */
/*           print_spaces(infos[i]->group, */
/*                        group_column - ft_strlen(infos[i]->group)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         print_number_spaces(infos[i]->lstats.st_size, */
/*                             size_column - */
/*                                 ft_number_len(infos[i]->lstats.st_size)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec); */
/*         char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1, */
/*                               ft_last_index_of(time, ':') - 1); */
/*         if (tmp != NULL) { */
/*           ft_putstr(tmp); */
/*           free(tmp); */
/*         } */
/**/
/*         ft_putchar(' '); */
/*         if (quote_space && ft_strchr(infos[i]->name, ' ') == NULL) */
/*           ft_putchar(' '); */
/**/
/*         print_dir_name(args, infos[i]->name, 0, true, false, false); */
/*         ft_putstr(" -> "); */
/**/
/*         char l_name[1000]; */
/*         ssize_t size = readlink(infos[i]->name, l_name, sizeof(l_name) - 1);
 */
/*         l_name[size] = '\0'; */
/**/
/*         if (size == -1) { */
/*           perror("readlink"); */
/**/
/*           return; */
/*         } */
/**/
/*         ft_putendl(l_name); */
/*       } else { */
/*         if (infos[i]->is_directory) */
/*           ft_putstr("d"); */
/*         else */
/*           ft_putstr("-"); */
/**/
/*         ft_putstr((infos[i]->stats.st_mode & S_IRUSR) ? "r" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IWUSR) ? "w" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IEXEC) ? "x" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IRGRP) ? "r" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IWGRP) ? "w" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IXGRP) ? "x" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IROTH) ? "r" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IWOTH) ? "w" : "-"); */
/*         ft_putstr((infos[i]->stats.st_mode & S_IXOTH) ? "x" : "-"); */
/**/
/*         ft_putchar(' '); */
/**/
/*         print_number_spaces(infos[i]->stats.st_nlink, */
/*                             link_size_column - */
/*                                 ft_number_len(infos[i]->stats.st_nlink)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         if (args->options.owner) { */
/*           print_spaces(infos[i]->owner, */
/*                        owner_column - ft_strlen(infos[i]->owner)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         if (args->options.group) { */
/*           print_spaces(infos[i]->group, */
/*                        group_column - ft_strlen(infos[i]->group)); */
/**/
/*           ft_putchar(' '); */
/*         } */
/**/
/*         print_number_spaces(infos[i]->stats.st_size, */
/*                             size_column - */
/*                                 ft_number_len(infos[i]->stats.st_size)); */
/**/
/*         ft_putchar(' '); */
/**/
/*         char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec); */
/*         char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1, */
/*                               ft_last_index_of(time, ':') - 1); */
/*         if (tmp != NULL) { */
/*           ft_putstr(tmp); */
/*           free(tmp); */
/*         } */
/**/
/*         ft_putchar(' '); */
/*         if (quote_space && ft_strchr(infos[i]->name, ' ') == NULL) */
/*           ft_putchar(' '); */
/**/
/*         int is_executable = infos[i]->stats.st_mode & S_IEXEC | */
/*                             infos[i]->stats.st_mode & S_IXGRP | */
/*                             infos[i]->stats.st_mode & S_IXOTH; */
/**/
/*         print_dir_name(args, infos[i]->name, 0, false,
 * infos[i]->is_directory, */
/*                        is_executable); */
/*         ft_putchar('\n'); */
/*       } */
/**/
/*       free(infos[i]->owner); */
/*       free(infos[i]->group); */
/*     } */
/*   } */
/* } */

void dir_default_list_printing(Args *args, ColumnValues *values, char **list,
                               size_t list_size, char *parent_dir) {
  if (args->options.listing) {
    /* listing_print(args, parent_dir); */
  } else {
    print_columns(args, values, list, list_size, parent_dir);
  }
}

void dir_recursive_list_printing(Args *args, char *dirname, int depth) {
  DIR *dir = opendir(dirname);

  if (dir == NULL) {
    perror("opendir");

    return;
  }

  if (depth > 0)
    ft_putchar('\n');

  depth++;
  print_dir(dirname);
  ft_putendl(":");

  /* dir_default_list_printing(args, dirname); */

  DIR *sub_dir = opendir(dirname);

  if (sub_dir == NULL) {
    perror("opendir");

    return;
  }

  struct dirent *dir_content;

  while ((dir_content = readdir(sub_dir))) {
    if (!args->options.hidden && dir_content->d_name[0] == '.')
      continue;

    char *tmp = ft_strjoin(dirname, "/");
    char *name = ft_strjoin(tmp, dir_content->d_name);

    free(tmp);

    struct stat statbuf;

    if (stat(name, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
      if (strcmp(dir_content->d_name, ".") != 0 &&
          strcmp(dir_content->d_name, "..") != 0) {
        dir_recursive_list_printing(args, name, depth);
      }
    }

    free(name);
  }

  closedir(sub_dir);
}

void dir_list_printing(Args *args, char *dirname) {
  if (args->options.recursive)
    dir_recursive_list_printing(args, dirname, 0);
  /* else */
  /* dir_default_list_printing(args, dirname); */
}

void print_dir_content(Args *args, char *dirname) {
  DIR *open_dir = opendir(dirname);

  if (!open_dir) {
    perror("opendir");

    return;
  }

  List *content = NULL;
  size_t length = 0;
  struct dirent *dir = NULL;
  ColumnValues values;

  init_columns_values(&values);

  while ((dir = readdir(open_dir))) {
    if (args->options.sort)
      ft_sorted_list_insert(&content, ft_strdup(dir->d_name), &str_cmp);
    else
      ft_list_push_back(&content, ft_strdup(dir->d_name));
    length++;
  }

  if (args->options.listing) {

  } else {
    char **list = ft_list_to_strs(content, length);

    ft_list_clear(content, &free);
    column_printing(args, &values, list, length, dirname);
  }
}

void list_detail_printing(Args *args, ColumnValues *values, List *list,
                          size_t list_size) {

  ft_putstr("total ");
  ft_putnbr(values->block_size);
  ft_putchar('\n');

  List *content = values->info;

  while (content != NULL) {
    Info *infos = content->data;

    if (infos->is_symlink) {
      ft_putstr("l");
      ft_putstr((infos->lstats.st_mode & S_IRUSR) ? "r" : "-");
      ft_putstr((infos->lstats.st_mode & S_IWUSR) ? "w" : "-");
      ft_putstr((infos->lstats.st_mode & S_IEXEC) ? "x" : "-");
      ft_putstr((infos->lstats.st_mode & S_IRGRP) ? "r" : "-");
      ft_putstr((infos->lstats.st_mode & S_IWGRP) ? "w" : "-");
      ft_putstr((infos->lstats.st_mode & S_IXGRP) ? "x" : "-");
      ft_putstr((infos->lstats.st_mode & S_IROTH) ? "r" : "-");
      ft_putstr((infos->lstats.st_mode & S_IWOTH) ? "w" : "-");
      ft_putstr((infos->lstats.st_mode & S_IXOTH) ? "x" : "-");

      ft_putchar(' ');

      print_number_spaces(infos->lstats.st_nlink,
                          values->link_size_column -
                              ft_number_len(infos->lstats.st_nlink));

      ft_putchar(' ');

      if (args->options.owner) {
        print_spaces(infos->owner,
                     values->owner_column - ft_strlen(infos->owner));

        ft_putchar(' ');
      }

      if (args->options.group) {
        print_spaces(infos->group,
                     values->group_column - ft_strlen(infos->group));

        ft_putchar(' ');
      }

      print_number_spaces(infos->lstats.st_size,
                          values->size_column -
                              ft_number_len(infos->lstats.st_size));

      ft_putchar(' ');

      char *time = ctime(&infos->lstats.st_mtim.tv_sec);
      char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1,
                            ft_last_index_of(time, ':') - 1);
      if (tmp != NULL) {
        ft_putstr(tmp);
        free(tmp);
      }

      ft_putchar(' ');
      if (values->quote_space && ft_strchr(infos->name, ' ') == NULL)
        ft_putchar(' ');

      print_dir_name(args, infos->name, 0, true, false, false);
      ft_putstr(" -> ");

      char l_name[1000];
      ssize_t size = readlink(infos->name, l_name, sizeof(l_name) - 1);
      l_name[size] = '\0';

      if (size == -1) {
        perror("readlink");

        return;
      }

      ft_putendl(l_name);
    } else {
      if (infos->is_directory)
        ft_putstr("d");
      else
        ft_putstr("-");

      ft_putstr((infos->stats.st_mode & S_IRUSR) ? "r" : "-");
      ft_putstr((infos->stats.st_mode & S_IWUSR) ? "w" : "-");
      ft_putstr((infos->stats.st_mode & S_IEXEC) ? "x" : "-");
      ft_putstr((infos->stats.st_mode & S_IRGRP) ? "r" : "-");
      ft_putstr((infos->stats.st_mode & S_IWGRP) ? "w" : "-");
      ft_putstr((infos->stats.st_mode & S_IXGRP) ? "x" : "-");
      ft_putstr((infos->stats.st_mode & S_IROTH) ? "r" : "-");
      ft_putstr((infos->stats.st_mode & S_IWOTH) ? "w" : "-");
      ft_putstr((infos->stats.st_mode & S_IXOTH) ? "x" : "-");

      ft_putchar(' ');

      print_number_spaces(infos->stats.st_nlink,
                          values->link_size_column -
                              ft_number_len(infos->stats.st_nlink));

      ft_putchar(' ');

      if (args->options.owner) {
        print_spaces(infos->owner,
                     values->owner_column - ft_strlen(infos->owner));

        ft_putchar(' ');
      }

      if (args->options.group) {
        print_spaces(infos->group,
                     values->group_column - ft_strlen(infos->group));

        ft_putchar(' ');
      }

      print_number_spaces(infos->stats.st_size,
                          values->size_column -
                              ft_number_len(infos->stats.st_size));

      ft_putchar(' ');

      char *time = ctime(&infos->lstats.st_mtim.tv_sec);
      char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1,
                            ft_last_index_of(time, ':') - 1);
      if (tmp != NULL) {
        ft_putstr(tmp);
        free(tmp);
      }

      ft_putchar(' ');
      if (values->quote_space && ft_strchr(infos->name, ' ') == NULL)
        ft_putchar(' ');

      int is_executable = infos->stats.st_mode & S_IEXEC |
                          infos->stats.st_mode & S_IXGRP |
                          infos->stats.st_mode & S_IXOTH;

      print_dir_name(args, infos->name, 0, false, infos->is_directory,
                     is_executable);
      ft_putchar('\n');
    }

    free(infos->owner);
    free(infos->group);

    content = content->next;
  }
}

void calc_list_values() {}

void list_printing(Args *args, ColumnValues *values, List *list,
                   size_t list_size) {
  init_columns_values(values);

  calc_list_values(values, list, list_size);

  list_detail_printing(args, values, list, list_size);
}

void process_inputs(Args *args, char *parent_dir, bool is_files) {
  if (is_files) {
    char **list = ft_list_to_strs(args->files.list, args->files.size);

    ft_list_clear(args->files.list, &free);

    if (args->options.listing) {
    } else
      column_printing(args, &args->files.values, list, args->files.size,
                      parent_dir);

    list_free((void **)list);
  } else {
    List *list = args->dirs.list;

    while (list != NULL) {
      if (!args->options.recursive) {
        ft_putstr(list->data);
        ft_putstr(":\n");
      }

      print_dir_content(args, list->data);

      /* dir_list_printing(args, list->data); */

      if (list->next != NULL)
        ft_putchar('\n');

      list = list->next;
    }
  }
}

int main(int argc, char **argv, char **env) {
  /* for (int i = 0; env[i] != NULL; i++) { */
  /*   char **tmp = ft_split(env[i], '='); */
  /**/
  /*   if (ft_strcmp(tmp[0], "HOME") == 0) */
  /*     printf("%s\n", env[i]); */
  /* } */

  Args args;

  init_args(&args);

  if (parse_args(&args, argc, argv) != 0)
    return 2;

  if (args.dirs.size == 0 && args.files.size == 0)
    dir_list_printing(&args, ".");
  else {
    if (args.files.list) {
      if (args.files.size > 1 && args.options.sort)
        ft_list_sort(&args.files.list, &str_cmp);

      process_inputs(&args, ".", true);

      if (args.dirs.list)
        ft_putchar('\n');
    }

    if (args.dirs.list) {
      if (args.dirs.size > 1 && args.options.sort)
        ft_list_sort(&args.dirs.list, &str_cmp);

      process_inputs(&args, ".", false);
    }
  }

  /* clear_args(&args); */

  return 0;
}
