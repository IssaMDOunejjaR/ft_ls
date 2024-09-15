#ifndef FT_LIB
#define FT_LIB

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

int ft_putchar(int c);
int ft_putstr(char *str);
int ft_putendl(char *str);
void ft_putnbr(int number);
int ft_strcmp(const char *s1, const char *s2);
size_t ft_strlen(const char *s);
char *ft_strdup(const char *s);
char *ft_strcat(char *dst, const char *src);
char *ft_strcpy(char *dst, const char *src);
size_t table_length(char **table);
void table_free(char **table);
char **table_push(char **table, char *value);
char **ft_split(const char *str, char c);
char *ft_substr(const char *str, size_t start, size_t end);
char *ft_strchr(const char *s, int c);
char *ft_trim(char const *str, char const *set);
char *ft_strjoin(char *s1, char *s2);
char ft_to_lower(char c);
char *ft_to_lowercase(char *str);
size_t ft_number_len(int number);
int ft_index_of_char(const char *str, char c);
int ft_last_index_of(char *str, char c);
int ft_is_2d_string_array_equals(char **a, char **b);

#endif
