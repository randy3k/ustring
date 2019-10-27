#include <string.h>
#include "utf32.h"


int utf32_codelen(uint32_t cp) {
    if (cp <= 0x10FFFF) {
        return 4;
    } else {
        return 0;
    }
}

int utf32_encode1_little(const unsigned char* s, uint32_t* cp) {
    int m;
    uint32_t x = s[0] + s[1]*256 + s[2]*65536 + s[3]*16777216;
    if (x > 0x10FFFF) {
        m = 0;
        x = -1;
    } else {
        m = 4;
    }
    *cp = x;
    return m;
}


int utf32_encode1_big(const unsigned char* s, uint32_t* cp) {
    int m;
    uint32_t x = s[3] + s[2]*256 + s[1]*65536 + s[0]*16777216;
    if (x > 0x10FFFF) {
        m = 0;
        x = -1;
    } else {
        m = 4;
    }
    *cp = x;
    return m;
}

int utf32_decode1_little(uint32_t cp, unsigned char* s) {
    if (0 <= cp && cp <= 0x10FFFF) {
        s[0] = cp%256;
        s[1] = cp/256%256;
        s[2] = cp/65536%256;
        s[3] = cp/16777216%256;
        return 4;
    } else {
        return 0;
    }
}

int utf32_decode1_big(uint32_t cp, unsigned char* s) {
    if (0 <= cp && cp <= 0x10FFFF) {
        s[3] = cp%256;
        s[2] = cp/256%256;
        s[1] = cp/65536%256;
        s[0] = cp/16777216%256;
        return 4;
    } else {
        return 0;
    }
}


void utf32_cp_collector(
            const unsigned char* s, size_t n,
            void collect(uint32_t, void*, size_t), void* data,
            int le) {
    uint32_t cp = -1;
    const unsigned char* c = s;
    size_t i = 0;
    size_t j = 0;
    if (le) {
        while (i < n) {
            utf32_encode1_little(c, &cp);
            collect(cp, data, j);
            c += 4;
            i += 4;
            j++;
        }
    } else {
        while (i < n) {
            utf32_encode1_big(c, &cp);
            collect(cp, data, j);
            c += 4;
            i += 4;
            j++;
        }
    }
}
