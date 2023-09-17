#ifndef LIB
#define LIB

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

size_t ft_strlen(char *str);
char *ft_strjoin(char *s1, char *s2);
char *ft_strcpy(char *dst, const char *src);
void ft_putchar_fd(int fd, char c);
void ft_putstr_fd(int fd, char *str);
void ft_putendl_fd(int fd, char *str);
char *ft_strdup(char *str);
size_t tab_len(char **tab);
char **tab_add(char **tab, char *new_element);
void free_tab(char **tab);

#endif
