#ifndef UTF_NBYTES_H__
#define UTF_NBYTES_H__


#include <stdint.h>
#include "utf8.h"
#include "utf16.h"
#include "utf32.h"


typedef struct {
    size_t utf8;
    size_t utf16;
    size_t utf32;
} utfsize_t;

utfsize_t utf8_nbytes(const unsigned char* s, size_t n);

utfsize_t utf16_nbytes(const unsigned char* s, size_t n, int le);

utfsize_t utf32_nbytes(const unsigned char* s, size_t n, int le);


#endif /* end of include guard: UTF_NBYTES_H__ */
