#ifndef UTF8_H__
#define UTF8_H__

#include <stddef.h>

#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif

long utf8_len(const unsigned char* s, long n);

int utf8_encode1(const unsigned char* s, int* cp);
int utf8_decode1(const int cp, unsigned char* s);

void utf8_cp_collector(const unsigned char* s, long n, void collect(int, int, void*, long), void* data);

#endif /* end of include guard: UTF8_H__ */
