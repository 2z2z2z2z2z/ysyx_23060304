#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
//  panic("Not implemented");
  size_t i = 0;
  while (*s) {
    s++;
    i++;
  }
  return i;
}

char *strcpy(char *dst, const char *src) {
//  panic("Not implemented");
  char *res = dst;
  while (*src) {
    *dst = *src;
    dst++;
    src++;
  }
  *dst = *src;
  return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
//  panic("Not implemented");
  char *ret = dst;
  for (int i = 0; i < n; i++) {
    if (*src) {
      *dst = *src;
      dst++;
      src++;
    } else {
      *dst = '\0';
    }
  }
  return ret;
}

char *strcat(char *dst, const char *src) {
//  panic("Not implemented");
  char *ret = dst;
  while (*dst) {
    dst++;
  }
  while (*src) {
    *dst = *src;
    dst++;
    src++;
  }
  *dst = *src;
  return ret;
}

int strcmp(const char *s1, const char *s2) {
//  panic("Not implemented");
  while ((*s1 == *s2) && (*s1) && (*s2)) {
    s1++;
    s2++;
  }
  if (*s1 < *s2) {
    return -1;
  } else if (*s1 > *s2) {
    return 1;
  } else {
    return 0;
  }
}

int strncmp(const char *s1, const char *s2, size_t n) {
//  panic("Not implemented");
  for (int i = 0; i < n; ++i) {
    if (*s1 < *s2) {
      return -1;
    } else if (*s1 > *s2) {
      return 1;
    } else if (*s1 == '\0') {
      break;
    }
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
//  panic("Not implemented");
  void *ret = s;
  for (int i = 0; i < n; ++i) {
    *(char *) s = c;
    s = (char *) s + 1;
  }
  return ret;
}

void *memmove(void *dst, const void *src, size_t n) {
//  panic("Not implemented");
//  void *ret = dst;
  if (dst < src) {
    for (int i = 0; i < n; ++i) {
      *((char *) dst + i) = *((char *) src + i);
    }
  } else {
    for (int i = n - 1; i >= 0; ++i) {
      *((char *) dst + i) = *((char *) src + i);
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
//  panic("Not implemented");
  for (int i = 0; i < n; ++i) {
    *((char *) out + i) = *((char *) in + i);
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
//  panic("Not implemented");
  for (int i = 0; i < n; ++i) {
    if (*((char *) s1 + i) < *((char *) s2 + i)) {
      return -1;
    } else if (*((char *) s1 + i) > *((char *) s2 + i)) {
      return 1;
    }
  }
  return 0;
}

#endif
