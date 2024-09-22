#ifndef FT_LS
#define FT_LS

#include "../libcft/ds/list/ft_list.h"
#include "ft_lib.h"
#include <asm-generic/ioctls.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>
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

typedef enum { _FILE, _DIR, _SYMLIK, _NONE } RessourceType;

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
  unsigned long terminal_width;
  short quote_space;
  short gap;
  size_t *col_widths;
  size_t num_columns;
  size_t num_rows;
  List *info;
  size_t link_size_column;
  size_t owner_column;
  size_t group_column;
  size_t size_column;
  size_t block_size;
} ColumnValues;

typedef struct {
  List *list;
  size_t size;
  ColumnValues values;
} Params;

typedef struct {
  Params dirs;
  Params files;
  Options options;
} Args;

void print_dir(char *dirname);
void print_dir_name(Args *args, char *dirname, int size, bool is_symlink,
                    bool is_directory, bool is_executable);
size_t *arraycpy(size_t *arr, size_t size);
bool string_cmp(void *a, void *b);
void pretty_print(Args *args, char *dirname);

#endif
