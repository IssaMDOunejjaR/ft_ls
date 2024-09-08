#include "../headers/ft_lib.h"

char *ft_strcat(char *dst, const char *src) {
  size_t dst_len = ft_strlen(dst);
  size_t str_len = (ft_strlen(src) + dst_len + 1);

  char *str = malloc(sizeof(char) * str_len);

  ft_strcpy(str, dst);
  ft_strcpy(str + dst_len, src);

  str[str_len] = '\0';

  return str;
}
