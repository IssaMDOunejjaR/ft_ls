#include "headers/ft_lib.h"
#include <stdarg.h>
#include <stdio.h>

#define BUFFER_SIZE 50

void print(int fd, char *format, ...) {
  char buffer[BUFFER_SIZE];
  size_t idx = 0;

  va_list args;

  va_start(args, format);

  for (int i = 0; format[i] != '\0'; i++) {
    if (format[i] == '%') {
      int j = i + 1;

      if (format[j] == 's') {
        char *str = va_arg(args, char *);
      }
    } else {
      buffer[idx] = format[i];
      idx++;
    }

    if (idx == BUFFER_SIZE - 1 || format[i + 1] == '\0') {
      buffer[BUFFER_SIZE - 1] = '\0';
      ft_putstr(buffer);
      idx = 0;
    }
  }
}

int main(void) {
  print(1, "Test: %s\n");

  return 0;
}
