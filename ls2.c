#include "./headers/ft_lib.h"
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
#include <sys/types.h>
#include <sys/xattr.h>
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

void print_dir_name(char *dirname, bool is_symlink, bool is_directory,
                    bool is_executable) {
  char *colors[] = {FILE_COLOR, DIRECTORY_COLOR, SYMLINK_COLOR, EXECUTABLE};

  int index = is_symlink ? 2 : (is_directory ? 1 : (is_executable ? 3 : 0));

  ft_putstr(colors[index]);

  char *has_space = ft_strchr(dirname, ' ');

  if (has_space != NULL)
    ft_putchar('\'');

  ft_putstr(dirname);

  if (has_space != NULL)
    ft_putchar('\'');

  ft_putstr(WHITE);
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

size_t list_size(void **lst) {
  size_t size = 0;

  if (lst == NULL)
    return size;

  for (int i = 0; lst[i] != NULL; i++)
    size++;

  return size;
}

void list_free(void **lst) {
  for (int i = 0; lst[i] != NULL; i++)
    free(lst[i]);

  free(lst);
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

  /* list_free((void **)lst); */

  free(lst);

  return new_list;
}

int main(int argc, char **argv) {
  DIR *o_dir;
  int fd = -1;
  char *dirname = ".";

  if (argc > 1)
    dirname = argv[1];

  o_dir = opendir(dirname);

  if (o_dir == NULL) {
    if ((fd = open(dirname, O_RDONLY)) == -1) {
      write(2, "ft_ls: cannot open directory '", 30);
      write(2, dirname, strlen(dirname));
      write(2, "': ", 3);
      perror("");

      return -1;
    }
  }

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
    Info *info = malloc(sizeof(Info));

    if (stat(dir->d_name, &info->stats) == -1) {
      perror("stat");

      return -1;
    }

    if (lstat(dir->d_name, &info->lstats) == -1) {
      perror("lstat");

      return -1;
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

      print_spaces(infos[i]->owner, owner_column - ft_strlen(infos[i]->owner));

      ft_putchar(' ');

      print_spaces(infos[i]->group, group_column - ft_strlen(infos[i]->group));

      ft_putchar(' ');

      print_number_spaces(infos[i]->lstats.st_size,
                          size_column -
                              ft_number_len(infos[i]->lstats.st_size));

      ft_putchar(' ');

      char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec);
      char *tmp = ft_substr(time, 0, ft_last_index_of(time, ':') - 1);
      if (tmp != NULL) {
        ft_putstr(tmp);
        free(tmp);
      }

      ft_putchar(' ');
      if (quote_space && ft_strchr(infos[i]->name, ' ') == NULL)
        ft_putchar(' ');

      print_dir_name(infos[i]->name, true, false, false);
      ft_putstr(" -> ");

      char l_name[1000];
      ssize_t size = readlink(infos[i]->name, l_name, sizeof(l_name) - 1);
      l_name[size] = '\0';

      if (size == -1) {
        perror("readlink");

        return -1;
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

      print_spaces(infos[i]->owner, owner_column - ft_strlen(infos[i]->owner));

      ft_putchar(' ');

      print_spaces(infos[i]->group, group_column - ft_strlen(infos[i]->group));

      ft_putchar(' ');

      print_number_spaces(infos[i]->stats.st_size,
                          size_column - ft_number_len(infos[i]->stats.st_size));

      ft_putchar(' ');

      char *time = ctime(&infos[i]->lstats.st_mtim.tv_sec);
      char *tmp = ft_substr(time, 0, ft_last_index_of(time, ':') - 1);
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

      print_dir_name(infos[i]->name, false, infos[i]->is_directory,
                     is_executable);
      ft_putchar('\n');
    }

    free(infos[i]->owner);
    free(infos[i]->group);
  }

  if (fd != -1)
    close(fd);

  list_free((void **)infos);
  closedir(o_dir);

  return 0;
}
