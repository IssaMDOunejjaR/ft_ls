#ifndef FT_LS
#define FT_LS

#include "libcft/ds/list/ft_list.h"
#include "libcft/io/ft_io.h"
#include "libcft/number/ft_number.h"
#include "libcft/string/ft_string.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <time.h>

#define WHITE "\033[0;37m"
#define BLACK "\033[0;30m"
#define CYAN "\033[1;36m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"

#define RESET_BACK "\x1b[0m"
#define GREEN_BACK "\x1b[42m"

#define FILE_COLOR WHITE
#define DIRECTORY_COLOR BLUE
#define SYMLINK_COLOR CYAN
#define EXECUTABLE_COLOR GREEN
#define STICKY_BIT_COLOR GREEN_BACK

typedef void (*ClearFunc)(void *);
typedef int (*CompareFunc)(void *, void *);
typedef void (*FormatFunc)(void *, void *);

enum Filetype { directory, normal, symbolic_link, executable, sticky_bit };

enum SortType {
  not_sort = -1,
  sort_none,
  sort_name,
  sort_update_time,
  sort_access_time,
  sort_size
};

enum Format { long_format, one_per_line, many_per_line, with_commas };

// Information about filling a column
typedef struct {
  bool print_quote;
  bool print_acl;

  // long_format info
  size_t nlink_width;
  size_t owner_width;
  size_t group_width;
  size_t size_width;
  size_t block_size;

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
  char *fullname;

  bool print_quote;
  bool print_acl;
  bool has_single_quote;

  char *owner_name;
  char *group_name;

  struct stat *stat;

  enum Filetype filetype;
} FileInfo;

typedef struct {
  List *list;
  size_t size;
  ColumnInfo column_info;
  bool error;
} Input;

typedef struct OwnerCache {
  uid_t uid;
  char *owner_name;
  struct OwnerCache *next;
} OwnerCache;

typedef struct GroupCache {
  gid_t gid;
  char *group_name;
  struct GroupCache *next;
} GroupCache;

// Compare Functions
int file_info_cmp_by_name(FileInfo *a, FileInfo *b);
int file_info_cmp_by_size(FileInfo *a, FileInfo *b);
int file_info_cmp_by_update_time(FileInfo *a, FileInfo *b);
int file_info_cmp_by_access_time(FileInfo *a, FileInfo *b);

#endif
