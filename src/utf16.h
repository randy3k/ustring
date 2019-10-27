#ifndef UTF16_H__
#define UTF16_H__

#include <stddef.h>
#include <stdint.h>

#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif

int utf16_codelen(uint32_t cp);

size_t utf16_length_little(const unsigned char* s, size_t n);
size_t utf16_length_big(const unsigned char* s, size_t n);

int utf16_encode1_little(const unsigned char* s, uint32_t* cp);
int utf16_encode1_big(const unsigned char* s, uint32_t* cp);
int utf16_decode1_little(uint32_t cp, unsigned char* s);
int utf16_decode1_big(uint32_t cp, unsigned char* s);

void utf16_cp_collector(
    const unsigned char* s, size_t n,
    void collect(uint32_t, void*, size_t), void* data,
    int le);

#endif /* end of include guard: UTF16_H__ */
