#ifndef UTF8_H__
#define UTF8_H__

#include <stddef.h>
#include <stdint.h>

#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif

int utf8_codelen(uint32_t cp);
/*
@param s utf8 string
@param n the length of the string
*/
size_t utf8_length(const unsigned char* s, size_t n);

int utf8_encode1(const unsigned char* s, uint32_t* cp);
int utf8_decode1(const uint32_t cp, unsigned char* s);

/*
@param s UTF-8 string
@param n the length of the string
@param collect a callback function,
            the first arg is the code point,
            the second arg is the number of code unit
            the third arg is the user data
            the forth arg is the iteration number
@param data user data passed to [collect]
*/
void utf8_cp_collector(const unsigned char* s, size_t n, void collect(uint32_t, void*, size_t), void* data);

#endif /* end of include guard: UTF8_H__ */
