#ifndef FT_LS
#define FT_LS

#include "lib/lib.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <time.h>
#include <unistd.h>

typedef struct s_params {
  char **paths;
  char *options;
} t_params;

void parse_params(t_params *params, int argc, char **argv);
void init_params(t_params *params);
void parse_options(t_params *params, char *options);
void no_dir_error(char *name);
void free_params(t_params *params);
void print_dir_content(DIR *dir, t_params *params, char *dirname);

#endif
