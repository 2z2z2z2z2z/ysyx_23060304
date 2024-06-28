#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <string.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

void int2str(int i, char *s) {
  char tmp[2] = {'\0', '\0'};
  int rem = i % 10;
  i = i / 10;
  if (i) {
    int2str(i, s);
  }
  tmp[0] = rem + '0';
  strcat(s, tmp);
}

int sprintf(char *out, const char *fmt, ...) {
//  panic("Not implemented");
  out[0]='\0';
  va_list va;
  int ret = 0;
  int d;
  char *s;
  char tmp[2] = {'\0', '\0'};
  va_start(va, fmt);
  while (*fmt) {
    switch (*fmt) {
      case 's':
        if (*(fmt - 1) == '%') {
          s = va_arg(va, char *);
          strcat(out, s);
          ret += strlen(s);
        } else {
          tmp[0] = *fmt;
          strcat(out, tmp);
          ret += 1;
        }
        break;
      case 'd':
        if (*(fmt - 1) == '%') {
          d = va_arg(va, int);
          char i2s[11]="";
          if (d < 0) {
            d = -d;
            strcat(out, "-");
            int2str(d, i2s);
            strcat(out, i2s);
          } else {
            int2str(d, i2s);
            strcat(out, i2s);
          }
        } else {
          tmp[0] = *fmt;
          strcat(out, tmp);
          ret += 1;
        }
        break;
      case '%':
        break;
      default:
        tmp[0] = *fmt;
        strcat(out, tmp);
        ret += 1;
    }
    fmt += 1;
  }
  va_end(va);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
