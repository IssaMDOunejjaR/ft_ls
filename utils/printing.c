#include "../headers/ft_ls.h"

/* void print_dir(char *dirname) { */
/*   char *has_space = ft_strchr(dirname, ' '); */
/**/
/*   if (has_space != NULL) { */
/*     ft_putchar('\''); */
/*     ft_putstr(dirname); */
/*     ft_putchar('\''); */
/*   } else */
/*     ft_putstr(dirname); */
/* } */
/**/
/* void print_dir_name(Args *args, char *dirname, int size, bool is_symlink, */
/*                     bool is_directory, bool is_executable) { */
/*   char *colors[] = {FILE_COLOR, DIRECTORY_COLOR, SYMLINK_COLOR, EXECUTABLE};
 */
/**/
/*   int index = */
/*       args->options.color */
/*           ? is_symlink ? 2 : (is_directory ? 1 : (is_executable ? 3 : 0)) */
/*           : 0; */
/**/
/*   ft_putstr(colors[index]); */
/**/
/*   print_dir(dirname); */
/**/
/*   ft_putstr(WHITE); */
/**/
/*   while (size > 0) { */
/*     ft_putchar(' '); */
/**/
/*     size--; */
/*   } */
/* } */
/**/
/* size_t *arraycpy(size_t *arr, size_t size) { */
/*   if (arr == NULL) */
/*     return NULL; */
/**/
/*   size_t *new_arr = malloc(size); */
/**/
/*   memcpy(new_arr, arr, size); */
/**/
/*   return new_arr; */
/* } */
/**/
/* bool string_cmp(void *a, void *b) { */
/*   char *tmp_a = ft_to_lowercase(a); */
/*   char *tmp_b = ft_to_lowercase(b); */
/**/
/*   if (ft_strcmp(tmp_a, tmp_b) > 0) { */
/*     free(tmp_a); */
/*     free(tmp_b); */
/**/
/*     return true; */
/*   } */
/**/
/*   free(tmp_a); */
/*   free(tmp_b); */
/**/
/*   return false; */
/* } */
/**/
/* void pretty_print(Args *args, char *name) { */
/*   DIR *o_dir = opendir(name); */
/*   struct winsize w; */
/**/
/*   if (ioctl(0, TIOCGWINSZ, &w) == -1) { */
/*     perror("ioctl"); */
/**/
/*     return; */
/*   } */
/**/
/*   char **strings = NULL; */
/*   struct dirent *dir; */
/*   int num_strings = 0; */
/*   int quote_space = 0; */
/*   int gap = 2; */
/**/
/*   while ((dir = readdir(o_dir))) { */
/*     if (!args->options.hidden && dir->d_name[0] == '.') */
/*       continue; */
/**/
/*     strings = table_push(strings, ft_strdup(dir->d_name)); */
/**/
/*     if (quote_space == 0 && strchr(dir->d_name, ' ') != NULL) { */
/*       quote_space = 1; */
/*       gap++; */
/*     } */
/**/
/*     num_strings++; */
/*   } */
/**/
/*   if (strings == NULL) */
/*     return; */
/**/
/*   strings = (char **)ft_bubble_sort((void **)strings, &string_cmp); */
/**/
/*   int terminal_width = w.ws_col; */
/**/
/*   int num_columns = 1; */
/*   int num_rows = num_strings; */
/*   size_t *col_widths = NULL, *tmp = NULL; */
/**/
/*   while (1) { */
/*     num_rows = (num_strings + num_columns - 1) / num_columns; */
/*     free(tmp); */
/*     tmp = arraycpy(col_widths, sizeof(int) * (num_columns - 1)); */
/*     free(col_widths); */
/*     col_widths = malloc(sizeof(int) * num_columns); */
/*     int total_width = quote_space; */
/**/
/*     for (int col = 0; col < num_columns; col++) { */
/*       int max_len = 0; */
/**/
/*       for (int row = 0; row < num_rows; row++) { */
/*         int index = row + col * num_rows; */
/**/
/*         if (index < num_strings) { */
/*           int len = strlen(strings[index]); */
/**/
/*           if (strchr(strings[index], ' ') != NULL) */
/*             len++; */
/**/
/*           if (len > max_len) { */
/*             max_len = len; */
/*           } */
/*         } */
/*       } */
/**/
/*       col_widths[col] = max_len; */
/*       total_width += max_len; */
/**/
/*       if (col < num_columns - 1) { */
/*         total_width += gap; */
/*       } */
/*     } */
/**/
/*     if (total_width >= terminal_width) { */
/*       if (num_columns > 1) { */
/*         free(col_widths); */
/**/
/*         col_widths = tmp; */
/*         num_columns--; */
/*         num_rows = (num_strings + num_columns - 1) / num_columns; */
/*       } */
/**/
/*       break; */
/*     } else if (num_columns == num_strings) { */
/*       free(tmp); */
/**/
/*       break; */
/*     } */
/**/
/*     num_columns++; */
/*   } */
/**/
/*   for (int row = 0; row < num_rows; row++) { */
/*     for (int col = 0; col < num_columns; col++) { */
/*       int index = row + col * num_rows; */
/**/
/*       if (index < num_strings) { */
/*         char *is_spaced_name = strchr(strings[index], ' '); */
/**/
/*         if (col == 0 && quote_space == 1 && is_spaced_name == NULL) */
/*           ft_putchar(' '); */
/**/
/*         int spaces = 0; */
/*         int next_word = row + (col + 1) * num_rows; */
/**/
/*         if (col < num_columns - 1) { */
/*           spaces = col_widths[col] - strlen(strings[index]) + gap; */
/**/
/*           if (next_word < num_strings && */
/*               strchr(strings[next_word], ' ') != NULL) { */
/*             spaces--; */
/*           } */
/*         } */
/**/
/*         if (is_spaced_name != NULL) */
/*           spaces--; */
/**/
/*         struct stat lstats, stats; */
/*         char *tmp = ft_strjoin(name, "/"); */
/*         char *path = ft_strjoin(tmp, strings[index]); */
/**/
/*         if (lstat(path, &lstats) == -1) { */
/*           perror("lstat"); */
/*           return; */
/*         } */
/**/
/*         if (stat(path, &stats) == -1) { */
/*           perror("stat"); */
/*           return; */
/*         } */
/**/
/*         free(tmp); */
/*         free(path); */
/**/
/*         bool is_symlink = false; */
/*         bool is_executable = false; */
/*         bool is_directory = false; */
/**/
/*         if (S_ISLNK(lstats.st_mode)) { */
/*           is_symlink = true; */
/*           is_executable = lstats.st_mode & S_IEXEC | lstats.st_mode & S_IXGRP
 * | */
/*                           lstats.st_mode & S_IXOTH; */
/*         } else { */
/*           if (S_ISDIR(stats.st_mode)) { */
/*             is_directory = true; */
/*           } else if (S_ISREG(stats.st_mode)) { */
/*             is_directory = false; */
/*           } */
/*           is_executable = lstats.st_mode & S_IEXEC | lstats.st_mode & S_IXGRP
 * | */
/*                           lstats.st_mode & S_IXOTH; */
/*         } */
/**/
/*         print_dir_name(args, strings[index], spaces, is_symlink,
 * is_directory, */
/*                        is_executable); */
/*       } */
/*     } */
/**/
/*     printf("\n"); */
/*   } */
/**/
/*   free(col_widths); */
/*   table_free(strings); */
/**/
/*   closedir(o_dir); */
/* } */
