#include <stdint.h>
#include "utf8.h"
#include "utf16.h"
#include "utf32.h"


typedef struct {
    size_t utf8;
    size_t utf16;
    size_t utf32;
} utf_size_t;

utf_size_t utf8_nbytes(const unsigned char* s, size_t n);
