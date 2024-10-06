#include "libcft/ds/list/ft_list.h"
#include "libcft/io/ft_io.h"
#include "libcft/number/ft_number.h"
#include "libcft/string/ft_string.h"
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>

#define MIN_COLUMN_WIDTH 3

#define WHITE "\033[0;37m"
#define CYAN "\033[0;36m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"

#define FILE_COLOR WHITE
#define DIRECTORY_COLOR BLUE
#define SYMLINK_COLOR CYAN
#define EXECUTABLE_COLOR GREEN

typedef void (*ClearFunc)(void *);
typedef int (*ComparFun)(void *, void *);
typedef void (*FormatFun)(void *, void *);

static int exit_status;

// Options
static bool sort_reverse;
static bool print_owner = true;
static bool print_group = true;
static bool print_with_color;
static bool recursive;
static bool immediate_dirs;
static bool ignore_hidden_files = true;

static bool format_needs_stat;
static bool format_needs_type;

enum Filetype { directory, normal, symbolic_link, executable };

static char *filetype_color[] = {DIRECTORY_COLOR, FILE_COLOR, SYMLINK_COLOR,
                                 EXECUTABLE_COLOR};

static char filetype_letter[] = "d-l-";

enum SortType { sort_none = -1, sort_name, sort_time };

enum Format { long_format, one_per_line, many_per_line, with_commas };

// Information about filling a column
typedef struct {
  bool print_quote;

  // long_format info
  size_t nlink_width;
  size_t owner_width;
  size_t group_width;
  size_t size_width;

  // many_per_line info
  size_t term_width;
  size_t num_columns;
  size_t num_rows;
  size_t gap;
  size_t *col_widths;
} ColumnInfo;

// Informations about a file
typedef struct {
  char *name;
  char *linkname;
  char *fullname;

  struct stat *stat;

  enum Filetype filetype;
} FileInfo;

typedef struct {
  List *list;
  size_t size;
  ColumnInfo column_info;
} Input;

enum SortType sort_type = sort_name;
enum Format format = many_per_line;
void (*out_format)(void *, void *);

Input all, files, dirs;

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
    {"-h", "--human-readable",
     "with -l and -s, print sizes like 1K 234M 2G etc.", NULL},
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
    {"-x", NULL, "list entries by lines instead of by columns", NULL},
    {"-1", NULL, "list one file per line", NULL},

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

void print_file_name(char *name, enum Filetype file_type) {
  size_t capacity = 20;
  char out[capacity];
  size_t pos = 0;

  if (print_with_color)
    output_buffering(out, &pos, capacity, filetype_color[file_type]);

  output_buffering(out, &pos, capacity, name);
  output_buffering(out, &pos, capacity, WHITE);

  ft_putstr(1, out);
}

void init_inputs(void) {
  files.list = NULL;
  files.size = 0;

  dirs.list = NULL;
  dirs.size = 0;
}

void clear_file_info(FileInfo *file_info) {
  free(file_info->name);
  free(file_info->linkname);
  free(file_info->fullname);
  /* free(file_info->stat); */

  free(file_info);
}

void clear_inputs(void) {
  ft_list_clear(files.list, (ClearFunc)&clear_file_info);
  ft_list_clear(dirs.list, (ClearFunc)&clear_file_info);
}

void clear_column_info(ColumnInfo *column_info) {
  free(column_info->col_widths);
}

void print_help() {
  ft_putstr(1,
            "Usage: ft_ls [OPTION]... [FILE]...\nList information about the "
            "FILEs (the current directory by default).\n\nMandatory arguments "
            "to long options are mandatory for short options too.\n");

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

    for (int j = 0; j < (left_pad - option_len - start_pad) + middle_gap; j++)
      ft_putchar(1, ' ');

    int max_paragraph_len = 70, count = 0;

    for (int j = 0; VALID_OPTIONS[i][2][j] != '\0'; j++) {
      ft_putchar(1, VALID_OPTIONS[i][2][j]);
      count++;

      if (count == max_paragraph_len) {
        count = 0;
        ft_putchar(1, '\n');

        for (int k = 0; k < left_pad + middle_gap; k++)
          ft_putchar(1, ' ');
      }
    }

    ft_putchar(1, '\n');
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
    } else
      return 0;

    return 1;
  } else if (c == 'a') {
    ignore_hidden_files = false;
  } else if (c == 'd') {
    immediate_dirs = true;
  } else if (c == 'f') {
    sort_type = sort_none;
    print_with_color = false;
    format = many_per_line;
    ignore_hidden_files = false;
    sort_reverse = false;
  } else if (c == 'g') {
    print_owner = false;
    format = long_format;
  } else if (c == 'l') {
    format = long_format;
  } else if (c == 'r') {
    sort_reverse = true;
  } else if (c == 'R') {
    recursive = true;
  } else if (c == 't') {
    sort_type = sort_time;
  } else if (c == 'u') {
  } else if (c == 'U') {
    sort_type = sort_none;
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
  info->linkname = NULL;
  info->stat = stat;

  if (!S_ISDIR(stat->st_mode) && !S_ISLNK(stat->st_mode) &&
      (stat->st_mode & S_IEXEC | stat->st_mode & S_IXGRP |
       stat->st_mode & S_IXOTH))
    info->filetype = executable;
  else if (S_ISLNK(stat->st_mode))
    info->filetype = symbolic_link;
  else if (S_ISDIR(stat->st_mode))
    info->filetype = directory;
  else if (S_ISREG(stat->st_mode))
    info->filetype = normal;

  return info;
}

int parse_args(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (ft_strcmp(argv[i], "--help") == 0) {
      print_help();

      return 0;
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

      if (lstat(argv[i], stat) == -1)
        input_not_found(argv[i]);
      else {
        FileInfo *info = create_file_info(ft_strdup(argv[i]), stat);

        ft_list_push_back(&all.list, info);
        all.size++;
      }
    }
  }

  return 0;
}

int file_info_cmp_by_name(FileInfo *a, FileInfo *b) {
  int idx_a = ft_last_index_of(a->name, '/');
  int idx_b = ft_last_index_of(b->name, '/');

  return ft_strcmp_lowercase(idx_a == -1 ? a->name : &a->name[idx_a + 1],
                             idx_b == -1 ? b->name : &b->name[idx_b + 1]);
}

void sort_inputs(void) {
  if (sort_type == sort_time)
    ft_list_sort(&all.list, NULL);
  else if (sort_type == sort_name)
    ft_list_sort(&all.list, (ComparFun)&file_info_cmp_by_name);

  if (sort_reverse)
    ft_list_reverse(&all.list);
}

size_t get_terminal_width(void) {
  struct winsize w;

  if (ioctl(0, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");

    return 0;
  }

  return w.ws_col;
}

void init_column_info(ColumnInfo *column_info) {
  column_info->print_quote = false;

  column_info->group_width = 0;
  column_info->owner_width = 0;
  column_info->nlink_width = 0;
  column_info->size_width = 0;

  column_info->term_width = get_terminal_width();
  column_info->num_columns = 0;
  column_info->num_rows = 0;
  column_info->gap = 2;
  column_info->col_widths = NULL;
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
  init_column_info(&input->column_info);

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

    size_t total_width = 0;

    for (size_t col = 0; col < num_columns; col++) {
      size_t max_len = 0;

      for (size_t row = 0; row < num_rows; row++) {
        size_t index = row + col * num_rows;

        if (index < input->size) {
          size_t len = ft_strlen(list[index]->name);

          if (ft_strchr(list[index]->name, ' ') != NULL)
            len++;

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
  print_file_name(file_info->name, file_info->filetype);

  while (size > 0) {
    ft_putchar(1, ' ');

    size--;
  }
}

void out_column_format(Input input, FileInfo **list) {
  for (int row = 0; row < input.column_info.num_rows; row++) {
    for (int col = 0; col < input.column_info.num_columns; col++) {
      int index = row + col * input.column_info.num_rows;

      if (index < input.size) {
        FileInfo *file_info = list[index];

        char *is_spaced_name = strchr(file_info->name, ' ');

        if (col == 0 && is_spaced_name == NULL)
          ft_putchar(1, ' ');

        int spaces = 0;
        int next_word = row + (col + 1) * input.column_info.num_rows;

        if (col < input.column_info.num_columns - 1) {
          spaces = input.column_info.col_widths[col] - strlen(file_info->name) +
                   input.column_info.gap;

          if (next_word < input.size &&
              strchr(list[next_word]->name, ' ') != NULL) {
            spaces--;
          }
        }

        if (is_spaced_name != NULL)
          spaces--;

        print_dir_name(file_info, spaces);
      }
    }

    ft_putchar(1, '\n');
  }
}

void calc_long_format(Input *input) {
  init_column_info(&input->column_info);

  List *list = input->list;
  ColumnInfo *column_info = &input->column_info;

  while (list != NULL) {
    FileInfo *file_info = list->data;

    char *owner = (getpwuid(file_info->stat->st_uid))->pw_name;
    char *group = (getgrgid(file_info->stat->st_gid))->gr_name;

    size_t owner_len = ft_strlen(owner);

    if (owner_len > column_info->owner_width)
      column_info->owner_width = owner_len;

    size_t group_len = ft_strlen(group);

    if (group_len > column_info->group_width)
      column_info->group_width = group_len;

    size_t len = ft_unsigned_number_len(file_info->stat->st_nlink);

    if (len > column_info->nlink_width)
      column_info->nlink_width = len;

    size_t size_len = ft_signed_number_len(file_info->stat->st_size);

    if (size_len > column_info->size_width)
      column_info->size_width = size_len;

    if (ft_strchr(file_info->name, ' ') != NULL)
      column_info->print_quote = true;

    list = list->next;
  }
}

char *get_permission(bool condition, char *value) {
  return condition ? value : "-";
}

void out_long_format(Input input) {
  List *list = input.list;
  ColumnInfo column_info = input.column_info;
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
    output_buffering(out, &pos, capacity,
                     get_permission(file_info->stat->st_mode & S_IXOTH, "x"));

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
      char *owner = (getpwuid(file_info->stat->st_uid))->pw_name;
      output_buffering(out, &pos, capacity, owner);

      size = column_info.owner_width - ft_strlen(owner);

      while (size > 0) {
        output_buffering(out, &pos, capacity, " ");

        size--;
      }

      output_buffering(out, &pos, capacity, " ");
    }

    if (print_group) {
      char *group = (getgrgid(file_info->stat->st_gid))->gr_name;
      output_buffering(out, &pos, capacity, group);

      size = column_info.group_width - ft_strlen(group);

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

    char *time = ctime(&file_info->stat->st_mtim.tv_sec);
    char *tmp = ft_substr(time, ft_index_of(time, ' ') + 1,
                          ft_last_index_of(time, ':') - 1);
    if (tmp != NULL) {
      output_buffering(out, &pos, capacity, tmp);
      free(tmp);
    }

    output_buffering(out, &pos, capacity, " ");

    bool print_quote = ft_strchr(file_info->name, ' ') != NULL;

    if (column_info.print_quote && !print_quote)
      output_buffering(out, &pos, capacity, " ");

    if (print_with_color) {
      output_buffering(out, &pos, capacity,
                       filetype_color[file_info->filetype]);
      if (print_quote) {
        output_buffering(out, &pos, capacity, "'");
        output_buffering(out, &pos, capacity, file_info->name);
        output_buffering(out, &pos, capacity, "'");
      } else
        output_buffering(out, &pos, capacity, file_info->name);

      output_buffering(out, &pos, capacity, WHITE);
    } else {
      if (print_quote) {
        output_buffering(out, &pos, capacity, "'");
        output_buffering(out, &pos, capacity, file_info->name);
        output_buffering(out, &pos, capacity, "'");
      } else
        output_buffering(out, &pos, capacity, file_info->name);
    }

    while (size > 0) {
      output_buffering(out, &pos, capacity, " ");

      size--;
    }

    if (file_info->filetype == symbolic_link) {
      output_buffering(out, &pos, capacity, " -> ");

      char l_name[256];
      ssize_t size = readlink(file_info->name, l_name, sizeof(l_name) - 1);

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

void print_out_format(Input input) {
  if (format == long_format) {
    calc_long_format(&input);
    out_long_format(input);
  } else if (format == many_per_line) {
    FileInfo **list = input_list_to_table(input);

    calc_many_per_line_format(&input, list);
    out_column_format(input, list);

    clear_column_info(&input.column_info);
    free(list);
  }
}

void separate_input(void) {
  List *list = all.list;

  while (list != NULL) {
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
  }
}

void list_print(void *data) {
  FileInfo *info = data;

  printf("'%s' -> { fullname: %s, linkname: %s, size: %ld, last_update: %lu, "
         "type: %s }\n",
         info->name, info->fullname, info->linkname, info->stat->st_size,
         info->stat->st_mtim.tv_sec,
         info->filetype == normal || info->filetype == symbolic_link ? "FILE"
                                                                     : "DIR");
}

void process_dir_content(FileInfo *file_info, char *parent_dir_name,
                         int depth) {
  char *name = file_info
                   ? file_info->fullname ? file_info->fullname : file_info->name
                   : ".";

  DIR *o_dir = opendir(name);

  if (!o_dir) {
    perror("opendir");

    return;
  }

  if (recursive) {
    if (depth > 0)
      ft_putchar(1, '\n');
    depth++;

    bool print_quote = ft_strchr(name, ' ') != NULL;

    if (print_quote) {
      ft_putchar(1, '\'');
      ft_putstr(1, name);
      ft_putchar(1, '\'');
    } else
      ft_putstr(1, name);

    ft_putstr(1, ":\n");
  }

  size_t name_len = ft_strlen(name);
  size_t path_buffer_size = name_len + 256;
  char *full_path = malloc(path_buffer_size);

  if (!full_path) {
    closedir(o_dir);
    perror("malloc");

    return;
  }

  ft_strcpy(full_path, name);
  full_path[name_len] = '/';

  struct dirent *content;
  Input input;

  input.list = NULL;
  input.size = 0;

  ColumnInfo *column_info = &input.column_info;

  init_column_info(column_info);

  while ((content = readdir(o_dir))) {
    if (ignore_hidden_files && content->d_name[0] == '.')
      continue;

    ft_strcpy(full_path + name_len + 1, content->d_name);

    /* struct stat *stat = malloc(sizeof(struct stat)); */
    struct stat stat;

    if (lstat(full_path, &stat) == -1) {
      perror("lstat");
      continue;
    }

    FileInfo *info = create_file_info(ft_strdup(content->d_name), &stat);

    info->fullname = ft_strdup(full_path);

    if (print_owner) {
      char *owner = (getpwuid(info->stat->st_uid))->pw_name;

      size_t owner_len = ft_strlen(owner);

      if (owner_len > column_info->owner_width)
        column_info->owner_width = owner_len;
    }

    if (print_group) {
      char *group = (getgrgid(info->stat->st_gid))->gr_name;

      size_t group_len = ft_strlen(group);

      if (group_len > column_info->group_width)
        column_info->group_width = group_len;
    }

    size_t len = ft_unsigned_number_len(info->stat->st_nlink);

    if (len > column_info->nlink_width)
      column_info->nlink_width = len;

    size_t size_len = ft_signed_number_len(info->stat->st_size);

    if (size_len > column_info->size_width)
      column_info->size_width = size_len;

    if (ft_strchr(info->name, ' ') != NULL)
      column_info->print_quote = true;

    ft_list_push_back(&input.list, info);
    input.size++;
  }

  if (sort_type == sort_time)
    ft_list_sort(&input.list, NULL);
  else if (sort_type == sort_name)
    ft_list_sort(&input.list, (ComparFun)&file_info_cmp_by_name);

  if (sort_reverse)
    ft_list_reverse(&input.list);

  if (format == long_format)
    out_long_format(input);
  else if (format == many_per_line) {
    FileInfo **list = input_list_to_table(input);

    calc_many_per_line_format(&input, list);
    out_column_format(input, list);

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
        process_dir_content(list->data, file_info->fullname, depth);

      list = list->next;
    }
  }

  free(full_path);
  ft_list_clear(input.list, (ClearFunc)&clear_file_info);
  closedir(o_dir);
}

void process_inputs(void) {
  if (immediate_dirs)
    print_out_format(all);
  else {
    separate_input();

    if (files.size > 0)
      print_out_format(files);

    if (dirs.size > 0) {
      List *list = dirs.list;

      while (list != NULL) {
        FileInfo *info = list->data;

        process_dir_content(info, info->name, 0);

        list = list->next;
      }
    } else {
      process_dir_content(NULL, ".", 0);
    }
  }
}

int main(int argc, char *argv[]) {
  init_inputs();

  if (parse_args(argc, argv) == 0) {
    sort_inputs();
    process_inputs();
  }

  clear_inputs();

  return exit_status;
}
