#ifndef UTF32_H__
#define UTF32_H__

#include <stddef.h>
#include <stdint.h>

#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif


int utf32_encode1_little(const unsigned char* s, int* cp);
int utf32_encode1_big(const unsigned char* s, int* cp);
int utf32_decode1_little(int cp, unsigned char* s);
int utf32_decode1_big(int cp, unsigned char* s);

void utf32_cp_collector(
    const unsigned char* s, long n,
    void collect(int, int, void*, long), void* data,
    int le);

#endif /* end of include guard: UTF32_H__ */
