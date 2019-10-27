#include <math.h>
#include "utf8.h"


typedef struct {
    unsigned char mask;  // data stored in these bits
    unsigned char lead;  // leading bits
} utf8_mask;


static utf8_mask utf8_masks[] = {
    {0x7F, 0x00},  // {0b01111111, 0b00000000},
    {0x1F, 0xC0},  // {0b00011111, 0b11000000},
    {0x0F, 0XE0},  // {0b00001111, 0b11100000},
    {0x07, 0xF0}   // {0b00000111, 0b11110000}
};

static size_t UTF8_MASKS_LEN = sizeof(utf8_masks) / sizeof(utf8_masks[0]);


static_inline int utf8_is_continuation(const unsigned char c) {
    return (c >> 6) == 0x02;  // 0b00000010
}


static int utf8_charlen(const unsigned char c) {
    utf8_mask u;
    int i;
    for (i = 0; i < UTF8_MASKS_LEN; i++) {
        u = utf8_masks[i];
        if ((c & ~u.mask) == u.lead) {
            break;
        }
    }
    return i < UTF8_MASKS_LEN? i + 1 : 0;
}

int utf8_codelen(uint32_t cp) {
    int m = 0;
    if (0 <= cp && cp <= 0x7F) {
        m = 1;
    } else if (cp <= 0x7FF) {
        m = 2;
    } else if (cp <= 0xFFFF) {
        m = 3;
    } else if (cp <= 0x10FFFF) {
        m = 4;
    } else {
        m = 0;
    }
    return m;
}

size_t utf8_len(const unsigned char* s, size_t n) {
    size_t k = 0;
    int m, j;
    size_t i;
    const unsigned char* c = s;
    for (i = 0; i < n; ) {
        m = utf8_charlen(*c);
        for (j = 1; j < m; j++) {
            if (!utf8_is_continuation(*(c + j))) {
                m = 0;
                break;
            }
        }
        if (m > 0) {
            c += m;
            i += m;
        } else {
            c++;
            i++;
        }
        k++;
    }
    return k;
}


int utf8_encode1(const unsigned char* s, uint32_t* cp) {
    const unsigned char* c;
    int m, i;
    m = utf8_charlen(*s);
    if (!m) {
        *cp = -1;
        return 0;
    }
    utf8_mask u = utf8_masks[m - 1];
    int temp = *s & u.mask;
    for (c = s + 1, i = 1; i < m; c++, i++) {
        if (!utf8_is_continuation(*c)) {
            temp = -1;
            m = 0;
            break;
        }
        temp = (temp << 6) + (*c & 0X3F);  // 0b00111111
    }
    *cp = temp;
    return m;
}


int utf8_decode1(uint32_t cp, unsigned char* s) {
    int m = 0;
    if (0 <= cp && cp <= 0x7F) {
        s[0] = cp;
        m = 1;
    } else if (cp <= 0x7FF) {
        s[0] = 0xC0 + cp/64;
        s[1] = 0x80 + cp%64;
        m = 2;
    } else if (cp <= 0xFFFF) {
        s[0] = 0xE0 + cp/4096;
        s[1] = 0x80 + cp/64%64;
        s[2] = 0x80 + cp%64;
        m = 3;
    } else if (cp <= 0x10FFFF) {
        s[0] = 0xF0 + cp/262144;
        s[1] = 0x80 + cp/4096%64;
        s[2] = 0x80 + cp/64%64;
        s[3] = 0x80 + cp%64;
        m = 4;
    }
    return m;
}

void utf8_cp_collector(const unsigned char* s, size_t k, void collect(uint32_t, void*, size_t), void* data) {
    uint32_t cp = -1;
    int m;
    const unsigned char* t = s;
    int i;
    i = 0;
    while (i < k) {
        m = utf8_encode1(t, &cp);
        collect(cp, data, i);
        t += m ? m : 1;
        i++;
    }
}
