#include <string.h>
#include "utf32.h"


int utf32_encode1_little(const unsigned char* s, int* cp) {
    int m;
    int x = s[0] + s[1]*256 + s[2]*65536 + s[3]*16777216;
    if (x > 0x10FFFF) {
        m = 0;
        x = 0;
    } else {
        m = 4;
    }
    *cp = x;
    return m;
}


int utf32_encode1_big(const unsigned char* s, int* cp) {
    int m;
    int x = s[3] + s[2]*256 + s[1]*65536 + s[0]*16777216;
    if (x > 0x10FFFF) {
        m = 0;
        x = 0;
    } else {
        m = 4;
    }
    *cp = x;
    return m;
}

int utf32_decode1_little(int cp, unsigned char* s) {
    if (0 < cp && cp <= 0x10FFFF) {
        s[0] = cp%256;
        s[1] = cp/256%256;
        s[2] = cp/65536%256;
        s[3] = cp/16777216%256;
        return 4;
   } else {
        return 0;
   }
}

int utf32_decode1_big(int cp, unsigned char* s) {
    if (0 < cp && cp <= 0x10FFFF) {
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
            const unsigned char* s, long n,
            void collect(int, void*, long), void* data,
            int le) {
    int cp = 0;
    const unsigned char* t;
    int i;
    t = s;
    i = 0;
    if (le) {
        while (i < n) {
            utf32_encode1_little(t, &cp);
            collect(cp, data, i);
            t += 4;
            i++;
        }
    } else {
        while (i < n) {
            utf32_encode1_big(t, &cp);
            collect(cp, data, i);
            t += 4;
            i++;
        }
    }
}
