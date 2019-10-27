#include "utf_nbytes.h"

void utf_nbytes_callback(uint32_t cp, void* data, size_t i) {
    utf_size_t* lp = (utf_size_t*) data;
    lp->utf8 += utf8_codelen(cp);
    lp->utf16 += utf16_codelen(cp);
    lp->utf32 += utf32_codelen(cp);
}

utf_size_t utf8_nbytes(const unsigned char* s, size_t n) {
    utf_size_t l = {0, 0, 0};
    utf8_cp_collector(s, n, utf_nbytes_callback, &l);
    return l;
}

utf_size_t utf16_nbytes(const unsigned char* s, size_t n, int le) {
    utf_size_t l = {0, 0, 0};
    utf16_cp_collector(s, n, utf_nbytes_callback, &l, le);
    return l;
}

utf_size_t utf32_nbytes(const unsigned char* s, size_t n, int le) {
    utf_size_t l = {0, 0, 0};
    utf32_cp_collector(s, n, utf_nbytes_callback, &l, le);
    return l;
}
