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
  bool print_attr;
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

extern int exit_status;
extern char *program_name;
extern char *filetype_color[];
extern char *name_set;
extern char filetype_letter[];

extern bool sort_reverse;
extern bool print_owner;
extern bool print_group;
extern bool print_with_color;
extern bool recursive;
extern bool immediate_dirs;
extern bool ignore_hidden_files;
extern bool ignore_dots;
extern bool print_quotes;
extern bool print_access_time;
extern bool _u;

extern enum SortType sort_type;
extern enum Format format;

extern OwnerCache *owner_cache;
extern GroupCache *group_cache;

extern Input all, files, dirs;

// Init Functions
void init_column_info(ColumnInfo *column_info);
void init_inputs(void);
FileInfo *create_file_info(char *name, struct stat *stat);

// Options
void print_help();
int get_opt(char short_opt, char *long_opt);
int parse_args(int argc, char **argv);

// Process
void process_inputs(void);

// Printing
void print_short_option_not_found(char option);
void print_long_option_not_found(char *option);
void input_not_found(char *input);
void print_block_size(unsigned long size);
void print_error(char *input, char *message);
void print_dir_name(FileInfo *file_info, int size);
void print_current_dir_name(char *name);
void print_out_format(Input input);

// Short Format
void calc_many_per_line_format(Input *input, FileInfo **list);
void out_column_format(Input *input, FileInfo **list);

// Long Format
void out_long_format(Input *input);
void update_column_info(FileInfo *info, ColumnInfo *column_info);
void calc_long_format(Input *input);

// Compare Functions
int file_info_cmp_by_name(FileInfo *a, FileInfo *b);
int file_info_cmp_by_size(FileInfo *a, FileInfo *b);
int file_info_cmp_by_update_time(FileInfo *a, FileInfo *b);
int file_info_cmp_by_access_time(FileInfo *a, FileInfo *b);
void sort_inputs(Input *input);

// Clear Funtions
void clear_file_info(FileInfo *file_info);
void clear_inputs(void);
void clear_column_info(ColumnInfo *column_info);

// Util Functions
void output_buffering(char *arr, size_t *pos, size_t capacity, char *str);
char *get_owner_name(uid_t uid);
char *get_group_name(gid_t gid);
FileInfo **input_list_to_table(Input input);

#endif
