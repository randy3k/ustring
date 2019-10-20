#ifndef UTF8_H__
#define UTF8_H__

#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif

long utf8_len(const unsigned char* s, long n);

void utf8_collector(const unsigned char* s, int n, void collect(int, int, void*, int), void* data);


#endif /* end of include guard: UTF8_H__ */
