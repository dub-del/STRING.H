// #include <string.h>

#include "s21_string.h"

char* s21_strcat(char* dest, const char* str) {
    char* start = dest;
    while (*dest != '\0') {
        dest++;
    }
    while (*str != '\0') {
        *dest = *str;
        dest++;
        str++;
    }
    *dest = '\0';
    return (start);
}
/*int main() {
  char str1[] = "abcdefg";
  char str2[] = "hijk";
  s21_strcat(str1, str2);
  printf("%s\n", str1);
  strcat(str1, str2);
  printf("%s\n", str1);
  return 0;
}*/