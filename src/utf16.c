#include <string.h>
#include "utf16.h"


int utf16_codelen(uint32_t cp) {
    int m = 0;
    if ((0 <= cp && cp <= 0xD7FF) || (cp >= 0xE000 && cp <= 0xFFFF)) {
        m = 2;
    } else if (cp >= 0x10000 && cp <= 0x10FFFF) {
        m = 4;
    }
    return m;
}

size_t utf16_npt_little(const unsigned char* s, size_t n) {
    size_t k = 0;
    const unsigned char* c = s;
    size_t i;
    uint16_t l;
    uint16_t h;
    for (i = 0; i < n ; ) {
        h =  c[0] + 256 * c[1];
        if (h <= 0xD7FF || h >= 0xE000) {
            k += 1;
            i += 2;
            c += 2;
        } else if (h <= 0xDBFF) {
            // high surrogate
            l = c[2] + 256 * c[3];
            if (l >= 0xDC00 && l <= 0xDFFF) {
                // low surrogate
                k += 1;
                i += 4;
                c += 4;
            } else {
                // not low surrogate
                i += 2;
                c += 2;
            }
        } else {
            // invalid high surrogate
            i += 2;
            c += 2;
        }
    }
    return k;
}

size_t utf16_npt_big(const unsigned char* s, size_t n) {
    size_t k = 0;
    const unsigned char* c = s;
    size_t i;
    uint16_t l;
    uint16_t h;
    for (i = 0; i < n ; ) {
        h = 256 * c[0] + c[1];
        if (h <= 0xD7FF || h >= 0xE000) {
            k += 1;
            i += 2;
            c += 2;
        } else if (h <= 0xDBFF) {
            // high surrogate
            l = 256 * c[2] + c[3];
            if (l >= 0xDC00 && l <= 0xDFFF) {
                // low surrogate
                k += 1;
                i += 4;
                c += 4;
            } else {
                // not low surrogate
                i += 2;
                c += 2;
            }
        } else {
            // invalid high surrogate
            i += 2;
            c += 2;
        }
    }
    return k;
}

int utf16_encode1_little(const unsigned char* s, uint32_t* cp) {
    int m = 0;
    uint16_t h = s[0] + 256 * s[1];
    uint16_t l;
    if (h <= 0xD7FF || h >= 0xE000) {
        // U+0000 to U+D7FF or U+E000 to U+FFFF
        *cp = h;
        m = 2;
    } else if (h <= 0xDBFF) {
        // high surrogate
        l = s[2] + 256 * s[3];
        if (l >= 0xDC00 && l <= 0xDFFF) {
            // low surrogate
            *cp = (h - 0xD800) * 0x400 + l - 0xDC00 + 0x10000;
            m = 4;
        } else {
            *cp = -1;
        }
    } else {
        *cp = -1;
    }
    return m;
}


int utf16_encode1_big(const unsigned char* s, uint32_t* cp) {
    int m = 0;
    uint16_t h = 256 * s[0] + s[1];
    uint16_t l;
    if (h <= 0xD7FF || h >= 0xE000) {
        // U+0000 to U+D7FF or U+E000 to U+FFFF
        *cp = h;
        m = 2;
    } else if (h <= 0xDBFF) {
        // high surrogate
        l = 256 * s[2] + s[3];
        if (l >= 0xDC00 && l <= 0xDFFF) {
            // low surrogate
            *cp = (h - 0xD800) * 0x400 + l - 0xDC00 + 0x10000;
            m = 4;
        } else {
            *cp = -1;
        }
    } else {
        *cp = -1;
    }
    return m;
}

int utf16_decode1_little(uint32_t cp, unsigned char* s) {
    int m = 0;
    if ((0 <= cp && cp <= 0xD7FF) || (cp >= 0xE000 && cp <= 0xFFFF)) {
        // one code unit
        s[0] = cp%256;
        s[1] = cp/256;
        m = 2;
    } else if (cp >= 0x10000 && cp <= 0x10FFFF) {
        uint16_t h;
        uint16_t l;
        cp -= 0x10000;
        l = cp%0x400 + 0xDC00;
        h = cp/0x400 + 0xD800;
        s[0] = h%256;
        s[1] = h/256;
        s[2] = l%256;
        s[3] = l/256;
        m = 4;
    }
    return m;
}

int utf16_decode1_big(uint32_t cp, unsigned char* s) {
    int m = 0;
    if ((0 <= cp && cp <= 0xD7FF) || (cp >= 0xE000 && cp <= 0xFFFF)) {
        // one code unit
        s[1] = cp%256;
        s[0] = cp/256;
        m = 2;
    } else if (cp >= 0x10000 && cp <= 0x10FFFF) {
        uint16_t h;
        uint16_t l;
        cp -= 0x10000;
        l = cp%0x400 + 0xDC00;
        h = cp/0x400 + 0xD800;
        s[0] = h/256;
        s[1] = h%256;
        s[2] = l/256;
        s[3] = l%256;
        m = 4;
    }
    return m;
}


void utf16_cp_collector(
            const unsigned char* s, size_t n,
            void collect(uint32_t, void*, size_t), void* data,
            int le) {
    uint32_t cp = -1;
    int m;
    const unsigned char* c = s;
    size_t i = 0;
    size_t j = 0;
    if (le) {
        while (i < n) {
            m = utf16_encode1_little(c, &cp);
            collect(cp, data, j);
            if (m) {
                c += m;
                i += m;
            } else {
                c += 2;
                i += 2;
            }
            j++;
        }
    } else {
        while (i < n) {
            m = utf16_encode1_big(c, &cp);
            collect(cp, data, j);
            if (m) {
                c += m;
                i += m;
            } else {
                c += 2;
                i += 2;
            }
            j++;
        }
    }
}
