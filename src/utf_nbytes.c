#include "utf_nbytes.h"

void utf8_nbytes_callback(uint32_t cp, void* data, size_t i) {
    utf_size_t* lp = (utf_size_t*) data;
    lp->utf8 += utf8_codelen(cp);
    lp->utf16 += utf16_codelen(cp);
    lp->utf32 += utf32_codelen(cp);
}

utf_size_t utf8_nbytes(const unsigned char* s, size_t n) {
    utf_size_t l;
    utf8_cp_collector(s, n, utf8_nbytes_callback, &l);
    return l;
}
