#include "nbytes.h"

void nbytes_callback(uint32_t cp, void* data, size_t i) {
    utfsize_t* lp = (utfsize_t*) data;
    lp->utf8 += utf8_codelen(cp);
    lp->utf16 += utf16_codelen(cp);
    lp->utf32 += utf32_codelen(cp);
}

utfsize_t utf8_nbytes(const unsigned char* s, size_t n) {
    utfsize_t l = {0, 0, 0};
    utf8_cp_collector(s, n, nbytes_callback, &l);
    return l;
}

utfsize_t utf16_nbytes(const unsigned char* s, size_t n, int le) {
    utfsize_t l = {0, 0, 0};
    utf16_cp_collector(s, n, nbytes_callback, &l, le);
    return l;
}

utfsize_t utf32_nbytes(const unsigned char* s, size_t n, int le) {
    utfsize_t l = {0, 0, 0};
    utf32_cp_collector(s, n, nbytes_callback, &l, le);
    return l;
}
