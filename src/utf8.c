#include <stdint.h>
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


static int is_ascii(const char* s) {
    const char* t;
    for (t = s; *t != '\0'; t++) {
        if (*t > 0x7F) {
            return 0;
        }
    }
    return 1;
}


static inline int utf8_is_continuation(const unsigned char c) {
    return (c >> 6) == 0x02;  // 0b00000010
}


static int utf8_cplen(const unsigned char c) {
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


int utf8_len(const char* s) {
    int n = 0;
    int m, j;
    const char* c;
    for (c = s; *c != '\0'; ) {
        m = utf8_cplen(*c);
        for (j = 1; j < m; j++) {
            if (!utf8_is_continuation(*(c + j))) {
                m = 0;
                break;
            }
        }
        c += m > 0? m : 1;
        n++;
    }
    return n;
}


static int utf8_decode1(const char* s, uint32_t* cp) {
    const char* c;
    int m, i;
    m = utf8_cplen(*s);
    if (!m) {
        return 0;
    }
    utf8_mask u = utf8_masks[m - 1];
    uint32_t temp = *s & u.mask;
    for (c = s + 1, i = 1; i < m; c++, i++) {
        if (*c == '\n' || !utf8_is_continuation(*c)) {
            return 0;
        }
        temp = (temp << 6) + (*c & 0X3F);  // 0b00111111
    }
    *cp = temp;
    return m;
}


const char* validate_utf8(SEXP s_) {
    if (TYPEOF(s_) != STRSXP || LENGTH(s_) != 1) {
        Rf_error("expect one-element character");
    }
    SEXP c = Rf_asChar(s_);
    const char* s = R_CHAR(c);
    if (!is_ascii(s) && Rf_getCharCE(c) != CE_UTF8) {
        Rf_error("non UTF-8 encoding");
    }
    return s;
}


SEXP C_utf8_len(SEXP s_) {
    PROTECT(s_);
    const char* s = validate_utf8(s_);;
    int n = utf8_len(s);
    UNPROTECT(1);
    return Rf_ScalarInteger(n);
}


SEXP C_utf8_decode(SEXP s_) {
    PROTECT(s_);
    const char* s = validate_utf8(s_);;
    int n = utf8_len(s);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, n));
    int* pt = INTEGER(p);
    uint32_t cp;
    int m;
    const char* t;
    int i;
    t = s;
    i = 0;
    while (i < n) {
        m = utf8_decode1(t, &cp);
        if (m) {
            pt[i] = cp;
            t = t + m;
        } else {
            pt[i] = NA_INTEGER;
            t++;
        }
        i++;
    }
    UNPROTECT(2);
    return p;
}


SEXP C_utf8_cplen(SEXP s_) {
    PROTECT(s_);
    const char* s = validate_utf8(s_);;
    int n = utf8_len(s);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, n));
    int* pt = INTEGER(p);
    uint32_t cp;
    int m;
    const char* t;
    int i;
    t = s;
    i = 0;
    while (i < n) {
        m = utf8_decode1(t, &cp);
        if (m) {
            pt[i] = m;
            t = t + m;
        } else {
            pt[i] = NA_INTEGER;
            t++;
        }
        i++;
    }
    UNPROTECT(2);
    return p;
}
