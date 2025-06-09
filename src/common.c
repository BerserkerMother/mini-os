#include "common.h"

size_t strlen(const char *data) {
  size_t len = 0;
  while (data[len]) {
    len++;
  }
  return len;
}
