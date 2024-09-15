#include "headers/ft_lib.h"
#include <asm-generic/ioctls.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define WHITE "\033[0;37m"
#define CYAN "\033[0;36m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"

#define FILE_COLOR WHITE
#define DIRECTORY_COLOR BLUE
#define SYMLINK_COLOR CYAN
#define EXECUTABLE GREEN

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

typedef struct {
  bool listing;
  bool hidden;
  bool sort;
  bool reverse;
  bool owner;
  bool group;
  bool directory;
  bool recursive;
  bool sort_by_time;
  bool color;
} Options;

typedef struct {
  char **inputs;
  Options options;
} Args;

void init_args(Args *args) {
  args->inputs = NULL;

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

void clear_args(Args *args) { table_free(args->inputs); }

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
  ft_putstr("ft_ls: invalid option -- '");
  ft_putchar(option);
  ft_putendl("'");
  ft_putendl("Try 'ft_ls --help' for more information.");
}

void print_long_option_not_found(char *option) {
  ft_putstr("ft_ls: unrecognized option '");
  ft_putstr(option);
  ft_putendl("'");
  ft_putendl("Try 'ft_ls --help' for more information.");
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
  ft_putstr("ft_ls: cannot access '");
  ft_putstr(input);
  ft_putendl("': No such file or directory");
}

int *arraycpy(int *arr, size_t size) {
  if (arr == NULL)
    return NULL;

  int *new_arr = malloc(size);

  memcpy(new_arr, arr, size);

  return new_arr;
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

typedef bool (*CompareFunc)(void *, void *b);

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

bool string_cmp(void *a, void *b) {
  char *tmp_a = ft_to_lowercase(a);
  char *tmp_b = ft_to_lowercase(b);

  if (ft_strcmp(tmp_a, tmp_b) > 0) {
    free(tmp_a);
    free(tmp_b);

    return true;
  }

  free(tmp_a);
  free(tmp_b);

  return false;
}

void **bubble_sort(void **list, CompareFunc cmp) {
  bool is_sorted = true;

  while (is_sorted) {
    is_sorted = false;

    for (int i = 0; list[i] != NULL; i++) {
      if (list[i + 1] != NULL && cmp(list[i], list[i + 1])) {
        void *tmp = list[i];

        list[i] = list[i + 1];
        list[i + 1] = tmp;

        is_sorted = true;
      }
    }
  }

  return list;
}

void print_dir_name(Args *args, char *dirname, int size, bool is_symlink,
                    bool is_directory, bool is_executable) {
  char *colors[] = {FILE_COLOR, DIRECTORY_COLOR, SYMLINK_COLOR, EXECUTABLE};

  int index =
      args->options.color
          ? is_symlink ? 2 : (is_directory ? 1 : (is_executable ? 3 : 0))
          : 0;

  ft_putstr(colors[index]);

  char *has_space = ft_strchr(dirname, ' ');

  if (has_space != NULL)
    ft_putchar('\'');

  ft_putstr(dirname);

  if (has_space != NULL)
    ft_putchar('\'');

  ft_putstr(WHITE);

  while (size > 0) {
    ft_putchar(' ');

    size--;
  }
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

void listing_print(Args *args, DIR *o_dir, char *dirname) {
  struct dirent *dir;
  Info **infos = NULL;
  int gap = 1;
  int quote_space = 0;
  int link_size_column = 0;
  int owner_column = 0;
  int group_column = 0;
  int size_column = 0;
  int block_size = 0;

  while ((dir = readdir(o_dir))) {
    if (!args->options.hidden && dir->d_name[0] == '.')
      continue;

    Info *info = malloc(sizeof(Info));

    char *tmp = ft_strjoin(dirname, "/");
    char *path = ft_strjoin(tmp, dir->d_name);

    if (stat(path, &info->stats) == -1) {
      perror("stat");

      return;
    }

    if (lstat(path, &info->lstats) == -1) {
      perror("lstat");

      return;
    }

    if (S_ISLNK(info->lstats.st_mode)) {
      info->is_symlink = true;

      info->owner = ft_strdup(getpwuid(info->lstats.st_uid)->pw_name);
      info->group = ft_strdup(getgrgid(info->lstats.st_gid)->gr_name);

      size_t owner_len = ft_strlen(info->owner);

      if (owner_len > owner_column)
        owner_column = owner_len;

      size_t group_len = ft_strlen(info->group);

      if (group_len > group_column)
        group_column = group_len;

      size_t len = ft_number_len(info->lstats.st_nlink);

      if (len > link_size_column)
        link_size_column = len;

      size_t size_len = ft_number_len(info->lstats.st_size);

      if (size_len > size_column)
        size_column = size_len;
    } else {
      block_size += info->stats.st_blocks;
      info->is_symlink = false;
      if (S_ISDIR(info->stats.st_mode)) {
        info->is_directory = true;
      } else if (S_ISREG(info->stats.st_mode)) {
        info->is_directory = false;
      }

      info->owner = ft_strdup(getpwuid(info->stats.st_uid)->pw_name);
      info->group = ft_strdup(getgrgid(info->stats.st_gid)->gr_name);

      size_t owner_len = ft_strlen(info->owner);

      if (owner_len > owner_column)
        owner_column = owner_len;

      size_t group_len = ft_strlen(info->group);

      if (group_len > group_column)
        group_column = group_len;

      size_t len = ft_number_len(info->stats.st_nlink);

      if (len > link_size_column)
        link_size_column = len;

      size_t size_len = ft_number_len(info->stats.st_size);

      if (size_len > size_column)
        size_column = size_len;
    }

    info->name = dir->d_name;

    if (ft_strchr(dir->d_name, ' ') != NULL)
      quote_space = 1;

    infos = list_append(infos, info);
  }

  ft_putstr("total ");
  ft_putnbr(block_size);
  ft_putchar('\n');
  if (infos) {
    infos = (Info **)bubble_sort((void **)infos, (CompareFunc)&info_cmp);

    for (int i = 0; infos[i] != NULL; i++) {
      if (infos[i]->is_symlink) {
        ft_putstr("l");
        ft_putstr((infos[i]->lstats.st_mode & S_IRUSR) ? "r" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IWUSR) ? "w" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IEXEC) ? "x" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IRGRP) ? "r" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IWGRP) ? "w" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IXGRP) ? "x" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IROTH) ? "r" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IWOTH) ? "w" : "-");
        ft_putstr((infos[i]->lstats.st_mode & S_IXOTH) ? "x" : "-");

        ft_putchar(' ');

        print_number_spaces(infos[i]->lstats.st_nlink,
                            link_size_column -
                                ft_number_len(infos[i]->lstats.st_nlink));

        ft_putchar(' ');

        if (args->options.owner) {
          print_spaces(infos[i]->owner,
                       owner_column - ft_strlen(infos[i]->owner));

          ft_putchar(' ');
        }

        if (args->options.group) {
          print_spaces(infos[i]->group,
                       group_column - ft_strlen(infos[i]->group));

          ft_putchar(' ');
        }

        print_number_spaces(infos[i]->lstats.st_size,
                            size_column -
                                ft_number_len(infos[i]->lstats.st_size));

        ft_putchar(' ');

        char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec);
        char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1,
                              ft_last_index_of(time, ':') - 1);
        if (tmp != NULL) {
          ft_putstr(tmp);
          free(tmp);
        }

        ft_putchar(' ');
        if (quote_space && ft_strchr(infos[i]->name, ' ') == NULL)
          ft_putchar(' ');

        print_dir_name(args, infos[i]->name, 0, true, false, false);
        ft_putstr(" -> ");

        char l_name[1000];
        ssize_t size = readlink(infos[i]->name, l_name, sizeof(l_name) - 1);
        l_name[size] = '\0';

        if (size == -1) {
          perror("readlink");

          return;
        }

        ft_putendl(l_name);
      } else {
        if (infos[i]->is_directory)
          ft_putstr("d");
        else
          ft_putstr("-");

        ft_putstr((infos[i]->stats.st_mode & S_IRUSR) ? "r" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IWUSR) ? "w" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IEXEC) ? "x" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IRGRP) ? "r" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IWGRP) ? "w" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IXGRP) ? "x" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IROTH) ? "r" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IWOTH) ? "w" : "-");
        ft_putstr((infos[i]->stats.st_mode & S_IXOTH) ? "x" : "-");

        ft_putchar(' ');

        print_number_spaces(infos[i]->stats.st_nlink,
                            link_size_column -
                                ft_number_len(infos[i]->stats.st_nlink));

        ft_putchar(' ');

        if (args->options.owner) {
          print_spaces(infos[i]->owner,
                       owner_column - ft_strlen(infos[i]->owner));

          ft_putchar(' ');
        }

        if (args->options.group) {
          print_spaces(infos[i]->group,
                       group_column - ft_strlen(infos[i]->group));

          ft_putchar(' ');
        }

        print_number_spaces(infos[i]->stats.st_size,
                            size_column -
                                ft_number_len(infos[i]->stats.st_size));

        ft_putchar(' ');

        char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec);
        char *tmp = ft_substr(time, ft_index_of_char(time, ' ') + 1,
                              ft_last_index_of(time, ':') - 1);
        if (tmp != NULL) {
          ft_putstr(tmp);
          free(tmp);
        }

        ft_putchar(' ');
        if (quote_space && ft_strchr(infos[i]->name, ' ') == NULL)
          ft_putchar(' ');

        int is_executable = infos[i]->stats.st_mode & S_IEXEC |
                            infos[i]->stats.st_mode & S_IXGRP |
                            infos[i]->stats.st_mode & S_IXOTH;

        print_dir_name(args, infos[i]->name, 0, false, infos[i]->is_directory,
                       is_executable);
        ft_putchar('\n');
      }

      free(infos[i]->owner);
      free(infos[i]->group);
    }
  }

  list_free((void **)infos);
  closedir(o_dir);
}

void pretty_print(Args *args, DIR *o_dir, char *dirname) {
  struct winsize w;

  if (ioctl(0, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");

    return;
  }

  char **strings = NULL;
  struct dirent *dir;
  int num_strings = 0;
  int quote_space = 0;
  int gap = 2;

  while ((dir = readdir(o_dir))) {
    if (!args->options.hidden && dir->d_name[0] == '.')
      continue;

    strings = table_push(strings, ft_strdup(dir->d_name));

    if (quote_space == 0 && strchr(dir->d_name, ' ') != NULL) {
      quote_space = 1;
      gap++;
    }

    num_strings++;
  }

  strings = (char **)bubble_sort((void **)strings, &string_cmp);

  int terminal_width = w.ws_col;

  int num_columns = 1;
  int num_rows = num_strings;
  int *col_widths = NULL;
  int *tmp = NULL;

  while (1) {
    num_rows = (num_strings + num_columns - 1) / num_columns;
    free(tmp);
    tmp = arraycpy(col_widths, sizeof(int) * (num_columns - 1));
    free(col_widths);
    col_widths = malloc(sizeof(int) * num_columns);
    int total_width = quote_space;

    for (int col = 0; col < num_columns; col++) {
      int max_len = 0;

      for (int row = 0; row < num_rows; row++) {
        int index = row + col * num_rows;

        if (index < num_strings) {
          int len = strlen(strings[index]);

          if (strchr(strings[index], ' ') != NULL)
            len++;

          if (len > max_len) {
            max_len = len;
          }
        }
      }

      col_widths[col] = max_len;
      total_width += max_len;

      if (col < num_columns - 1) {
        total_width += gap;
      }
    }

    if (total_width >= terminal_width) {
      if (num_columns > 1) {
        free(col_widths);

        col_widths = tmp;
        num_columns--;
        num_rows = (num_strings + num_columns - 1) / num_columns;
      }

      break;
    } else if (num_columns == num_strings)
      break;

    num_columns++;
  }

  for (int row = 0; row < num_rows; row++) {
    for (int col = 0; col < num_columns; col++) {
      int index = row + col * num_rows;

      if (index < num_strings) {
        char *is_spaced_name = strchr(strings[index], ' ');

        if (col == 0 && quote_space == 1 && is_spaced_name == NULL)
          ft_putchar(' ');

        int spaces = 0;
        int next_word = row + (col + 1) * num_rows;

        if (col < num_columns - 1) {
          spaces = col_widths[col] - strlen(strings[index]) + gap;

          if (next_word < num_strings &&
              strchr(strings[next_word], ' ') != NULL) {
            spaces--;
          }
        }

        if (is_spaced_name != NULL)
          spaces--;

        struct stat lstats, stats;
        char *tmp = ft_strjoin(dirname, "/");
        char *path = ft_strjoin(tmp, strings[index]);

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

        print_dir_name(args, strings[index], spaces, is_symlink, is_directory,
                       is_executable);
      }
    }

    printf("\n");
  }

  free(col_widths);
  table_free(strings);

  closedir(o_dir);
}

void dir_default_list_printing(Args *args, DIR *dir, char *dirname) {
  if (args->options.listing) {
    listing_print(args, dir, dirname);
  } else {
    pretty_print(args, dir, dirname);
  }
}

void dir_recursive_list_printing(Args *args, DIR *dir, char *dirname,
                                 int depth) {
  struct dirent *dir_content;

  if (depth > 0)
    ft_putchar('\n');

  depth++;

  ft_putstr(dirname);
  ft_putendl(":");

  if (args->options.listing) {
    listing_print(args, dir, dirname);
  } else {
    pretty_print(args, dir, dirname);
  }

  DIR *sub_dir = opendir(dirname);

  if (dir == NULL)
    perror("opendir");

  while ((dir_content = readdir(sub_dir))) {
    if (ft_strcmp(dir_content->d_name, ".") == 0 ||
        ft_strcmp(dir_content->d_name, "..") == 0)
      continue;

    if (!args->options.hidden && dir_content->d_name[0] == '.')
      continue;

    char *tmp = ft_strjoin(dirname, "/");
    char *name = ft_strjoin(tmp, dir_content->d_name);
    free(tmp);

    DIR *o_dir = opendir(name);

    if (o_dir == NULL) {
      free(name);

      continue;
    }

    dir_recursive_list_printing(args, o_dir, name, depth);
  }
}

void dir_list_printing(Args *args, DIR *dir, char *dirname) {
  if (args->options.recursive)
    dir_recursive_list_printing(args, dir, dirname, 0);
  else
    dir_default_list_printing(args, dir, dirname);
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

  for (int i = 1; i < argc; i++) {
    if (ft_strcmp(argv[i], "--help") == 0) {
      print_help();

      clear_args(&args);

      return 0;
    } else if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        if (check_long_option(&args, argv[i]) == 0) {
          print_long_option_not_found(argv[i]);

          return -1;
        }
      } else {
        for (int j = 1; argv[i][j] != '\0'; j++) {
          if (check_short_option(&args, argv[i][j]) == 0) {
            print_short_option_not_found(argv[i][j]);

            return -1;
          }
        }
      }
    } else {
      args.inputs = table_push(args.inputs, ft_strdup(argv[i]));
    }
  }

  /* print_options(&args); */
  /**/
  /* printf("Inputs:\n"); */
  /* if (args.inputs != NULL) { */
  /*   for (int i = 0; args.inputs[i] != NULL; i++) { */
  /*     printf(" - %s\n", args.inputs[i]); */
  /*   } */
  /* } */
  /* ft_putchar('\n'); */

  size_t inputs_len = table_length(args.inputs);

  if (inputs_len == 0) {
    DIR *opened_dir = opendir(".");

    dir_list_printing(&args, opened_dir, ".");
  } else if (inputs_len == 1) {
    DIR *opened_dir = opendir(args.inputs[0]);
    int fd = -1;

    if (opened_dir == NULL) {
      fd = open(args.inputs[0], O_RDONLY);

      if (fd == -1) {
        input_not_found(args.inputs[0]);

        return 2;
      }
    }

    if (opened_dir != NULL) {
      dir_list_printing(&args, opened_dir, args.inputs[0]);
    } else {
    }
  } else {
    for (int i = 0; args.inputs[i] != NULL; i++) {
      DIR *opened_dir = opendir(args.inputs[i]);
      int fd = -1;

      if (opened_dir == NULL) {
        fd = open(args.inputs[i], O_RDONLY);

        if (fd == -1) {
          input_not_found(args.inputs[i]);

          return 2;
        }
      }

      if (opened_dir != NULL) {
        ft_putstr(args.inputs[i]);
        ft_putendl(":");
        dir_list_printing(&args, opened_dir, args.inputs[i]);
      } else {
        if (args.options.listing) {

        } else {
        }
      }

      if (args.inputs[i + 1] != NULL)
        ft_putchar('\n');
    }
  }

  clear_args(&args);

  return 0;
}
