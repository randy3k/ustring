#include <stdint.h>
#include <string.h>
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


static int utf8_codelen1(const unsigned char c) {
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

/*
@param s utf8 string
@param n number of code points in string or -1 when it is unknown but the string is null terminated
*/
long utf8_len(const unsigned char* s, long n) {
    int k = 0;
    int m, j;
    long i;
    const unsigned char* c;
    for (i = 0, c = s; i < n || (n == -1 && *c != '\0'); ) {
        m = utf8_codelen1(*c);
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


static int utf8_code1(const unsigned char* s, int* cp) {
    const unsigned char* c;
    int m, i;
    m = utf8_codelen1(*s);
    if (!m) {
        return 0;
    }
    utf8_mask u = utf8_masks[m - 1];
    int temp = *s & u.mask;
    for (c = s + 1, i = 1; i < m; c++, i++) {
        if (!utf8_is_continuation(*c)) {
            return 0;
        }
        temp = (temp << 6) + (*c & 0X3F);  // 0b00111111
    }
    *cp = temp;
    return m;
}




/*
@param s utf-8 string
@param n number of code points in the string
@param collect a callback function,
            the first arg is the code point,
            the second arg is the number of code unit
            the third arg is the user data
            the forth arg is the iteration number
@param data user data passed to [collect]
*/
void utf8_collector(const unsigned char* s, int n, void collect(int, int, void*, int), void* data) {
    int cp = 0;
    int m;
    const unsigned char* t;
    int i;
    t = s;
    i = 0;
    while (i < n) {
        m = utf8_code1(t, &cp);
        if (m) {
            collect(cp, m, data, i);
            t = t + m;
        } else {
            collect(0, 1, data, i);
            t++;
        }
        i++;
    }
}
