#include "../headers/ft_lib.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_2d_string_array(char **arr) {
  ft_putstr("[ ");

  for (int i = 0; arr[i] != NULL; i++) {
    ft_putstr(arr[i]);

    if (arr[i + 1] != NULL)
      ft_putstr(", ");
  }

  ft_putstr(" ]");
}

void print_args(int count, ...) {
  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; i += 2) {
    const char *arg_name = va_arg(args, const char *);
    const char *arg_value = va_arg(args, const char *);

    printf("- %s = \"%s\"\n", arg_name, arg_value);
  }

  va_end(args);
}

void print_2d_args(int count, ...) {
  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; i += 2) {
    char *arg_name = va_arg(args, char *);
    char **arg_value = va_arg(args, char **);

    ft_putstr("- ");
    ft_putstr(arg_name);
    ft_putstr("  \"");
    print_2d_string_array(arg_value);
    ft_putstr("\"\n");
  }

  va_end(args);
}

#define CHECK_EQUAL_INT(EXPECTED, ACTUAL, ...)                                 \
  if (EXPECTED != ACTUAL) {                                                    \
    print_args(__VA_ARGS__);                                                   \
    fprintf(stderr, "For '%s', expected '%d', got '%d'\n", #ACTUAL, EXPECTED,  \
            ACTUAL);                                                           \
  }

#define CHECK_EQUAL_UNSIGNED_LONG(EXPECTED, ACTUAL, ...)                       \
  if (EXPECTED != ACTUAL) {                                                    \
    print_args(__VA_ARGS__);                                                   \
    fprintf(stderr, "For '%s', expected '%lu', got '%lu'\n", #ACTUAL,          \
            EXPECTED, ACTUAL);                                                 \
  }

#define CHECK_EQUAL_STRING(EXPECTED, ACTUAL, ...)                              \
  if (strcmp(EXPECTED, ACTUAL) != 0) {                                         \
    print_args(__VA_ARGS__);                                                   \
    fprintf(stderr, "For '%s', expected '%s', got '%s'\n", #ACTUAL, EXPECTED,  \
            ACTUAL);                                                           \
  }

#define CHECK_EQUAL_2D_STRING_ARRAY(EXPECTED, ACTUAL, ...)                     \
  if (ft_is_2d_string_array_equals(EXPECTED, ACTUAL) != 0) {                   \
    print_args(__VA_ARGS__);                                                   \
    fprintf(stderr, "For '%s', expected '%s', got '%s'\n", #ACTUAL, EXPECTED,  \
            ACTUAL);                                                           \
  }

char *random_string() {
  char *str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  int len_str = strlen(str);
  int length = rand() % 10;
  char *random_string = malloc(length * sizeof(char) + 1);

  if (!random_string)
    return NULL;

  int i = 0;

  while (i < length) {
    int random_char = rand() % len_str - 1;
    random_string[i] = str[random_char];

    i++;
  }

  random_string[i] = '\0';

  return random_string;
}

int main(void) {
  // strlen
  for (int i = 0; i < 10; i++) {
    char *s = random_string();

    CHECK_EQUAL_UNSIGNED_LONG(strlen(s), ft_strlen(s), 2, "s", s);

    free(s);
  }

  // strcmp
  for (int i = 0; i < 10; i++) {
    char *s1 = random_string();
    char *s2 = random_string();

    CHECK_EQUAL_INT(strcmp(s1, s2), ft_strcmp(s1, s2), 4, "s1", s1, "s2", s2);

    free(s1);
    free(s2);
  }

  // strdup
  for (int i = 0; i < 10; i++) {
    char *s = random_string();
    char *s1 = strdup(s);
    char *s2 = ft_strdup(s);

    CHECK_EQUAL_STRING(s1, s2, 2, "s", s);

    free(s);
    free(s1);
    free(s2);
  }

  // strcpy
  for (int i = 0; i < 10; i++) {
    char *dst = random_string();
    char *src = random_string();

    CHECK_EQUAL_STRING(strcpy(dst, src), ft_strcpy(dst, src), 4, "dst", dst,
                       "src", src);

    free(dst);
    free(src);
  }

  // strcat
  for (int i = 0; i < 10; i++) {
    char *dst = random_string();
    char *src = random_string();

    CHECK_EQUAL_STRING(strcat(dst, src), ft_strcat(dst, src), 4, "dst", dst,
                       "src", src);

    free(dst);
    free(src);
  }

  // substr
  {
    char *str = "Hello World!";
    char *expected = "lo W";
    char *substr = ft_substr(str, 3, 6);

    CHECK_EQUAL_STRING(expected, substr, 4, "str", str, "exp", expected);

    free(substr);
  }
  {
    char *str = "Hello World!";
    char *expected = "He";
    char *substr = ft_substr(str, 0, 1);

    CHECK_EQUAL_STRING(expected, substr, 4, "str", str, "exp", expected);

    free(substr);
  }

  // trim
  {
    char *str = "    Hello World!    ";
    char *expected = "Hello World!";
    char *set = " ";
    char *substr = ft_trim(str, set);

    CHECK_EQUAL_STRING(expected, substr, 6, "str", str, "exp", expected, "set",
                       set);

    free(substr);
  }
  {
    char *str = "  ----  Hello World!  - - -";
    char *expected = "Hello World!";
    char *set = " -";
    char *substr = ft_trim(str, set);

    CHECK_EQUAL_STRING(expected, substr, 6, "str", str, "exp", expected, "set",
                       set);

    free(substr);
  }
  {
    char *str = "Hello World!";
    char *expected = "Hello World!";
    char *set = " -";
    char *substr = ft_trim(str, set);

    CHECK_EQUAL_STRING(expected, substr, 6, "str", str, "exp", expected, "set",
                       set);

    free(substr);
  }
  {
    char *str = "123445243523452345Hello World!352345234523452452345";
    char *expected = "Hello World!";
    char *set = "1234567890";
    char *substr = ft_trim(str, set);

    CHECK_EQUAL_STRING(expected, substr, 6, "str", str, "exp", expected, "set",
                       set);

    free(substr);
  }
  {
    char *str = "123445243523452345Hello World!352345234523452452345";
    char *expected = "123445243523452345Hello World!352345234523452452345";
    char *set = "";
    char *substr = ft_trim(str, set);

    CHECK_EQUAL_STRING(expected, substr, 6, "str", str, "exp", expected, "set",
                       set);

    free(substr);
  }

  // strjoin
  {
    char *s1 = "Hello";
    char *s2 = " World!";
    char *expected = "Hello World!";
    char *actual = ft_strjoin(s1, s2);

    CHECK_EQUAL_STRING(expected, actual, 6, "s1", s1, "s2", s2, "expected",
                       expected);

    free(actual);
  }
  {
    char *s1 = "Hello";
    char *s2 = "World!";
    char *expected = "HelloWorld!";
    char *actual = ft_strjoin(s1, s2);

    CHECK_EQUAL_STRING(expected, actual, 6, "s1", s1, "s2", s2, "expected",
                       expected);

    free(actual);
  }
  {
    char *s1 = "Hello";
    char *s2 = NULL;
    char *expected = "Hello";
    char *actual = ft_strjoin(s1, s2);

    CHECK_EQUAL_STRING(expected, actual, 6, "s1", s1, "s2", s2, "expected",
                       expected);

    free(actual);
  }

  // number_len
  { printf("%zu\n", ft_number_len(10)); }

  // TODO: index_of_char
  // TODO: split
  // TODO: strchr

  /* char **actual; */
  /* char *expected[] = {"Hello", "World!", "issam", "ounejjar", NULL}; */
  /* actual = ft_split("Hello         World! issam ounejjar", ' '); */

  /* CHECK_EQUAL_2D_STRING_ARRAY(expected, actual, 4, ""); */

  /* printf("split: %d\n", ft_is_2d_string_array_equals(tmp, arr)); */

  return 0;
}
