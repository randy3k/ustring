#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include "utf8.h"

#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif

static_inline int is_little_endian() {
    int t = 1;
    return (*(char *)&t == 1);
}

static_inline uint32_t swapbit32(uint32_t num) {
    return ((num & 0xff000000) >> 24) | ((num & 0x00ff0000) >> 8) | ((num & 0x0000ff00) << 8) | (num << 24);
}

static int is_ascii(const unsigned char* s) {
    // FIXME: s could contain NULL
    const unsigned char* t;
    for (t = s; *t != '\0'; t++) {
        if (*t > 0x7F) {
            return 0;
        }
    }
    return 1;
}

static const unsigned char* validate_utf8(SEXP s_) {
    const unsigned char* s;
    if (TYPEOF(s_) == STRSXP ) {
        if (LENGTH(s_) != 1) Rf_error("expect one-element character");
        SEXP c = Rf_asChar(s_);
        s = (const unsigned char*) R_CHAR(c);
        if (!is_ascii(s) && Rf_getCharCE(c) != CE_UTF8) {
            Rf_error("non UTF-8 encoding");
        }
    } else if (TYPEOF(s_) == RAWSXP) {
        s = RAW(s_);
    } else {
        Rf_error("expect one-element character");
    }
    return s;
}


static_inline long utf8_len_(SEXP s_, const unsigned char* s) {
    long n;
    if (TYPEOF(s_) == STRSXP) {
        n = utf8_len(s, -1);
    } else {
        n = utf8_len(s, LENGTH(s_));
    }
    return n;
}

SEXP C_utf8_len(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);
    long n = utf8_len_(s_, s);
    UNPROTECT(1);
    return Rf_ScalarReal((double) n);
}


void utf8_codelen_callback(int cp, int m, void* data, int i) {
    int* pt = (int*) data;
    pt[i] = m? m : NA_INTEGER;
}

SEXP C_utf8_codelen(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    long n = utf8_len_(s_, s);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, n));
    int* pt = INTEGER(p);
    utf8_collector(s, n, utf8_codelen_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}


void utf8_code_callback(int cp, int m, void* data, int i) {
    int* pt = (int*) data;
    pt[i] = cp? cp : NA_INTEGER;
}

SEXP C_utf8_code(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    long n = utf8_len_(s_, s);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, n));
    int* pt = INTEGER(p);
    utf8_collector(s, n, utf8_code_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}

void utf8_to_utf32_callback(int cp, int m, void* data, int i) {
    uint32_t* pt = (uint32_t*) data;
    pt[i] = cp;
}

void utf8_to_utf32_callback2(int cp, int m, void* data, int i) {
    uint32_t* pt = (uint32_t*) data;
    pt[i] = swapbit32((uint32_t) cp);
}

SEXP C_utf8_to_utf32(SEXP s_, SEXP le_) {
    PROTECT(s_);
    int le = Rf_asLogical(le_);
    const unsigned char* s = validate_utf8(s_);;
    long n = utf8_len_(s_, s);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, n * sizeof(uint32_t) / sizeof(char)));
    uint32_t* pt = (uint32_t*) RAW(p);
    int machinele = is_little_endian();
    if ((le && machinele) || (!le && !machinele)) {
        utf8_collector(s, n, utf8_to_utf32_callback, (void*) pt);
    } else {
        utf8_collector(s, n, utf8_to_utf32_callback2, (void*) pt);

    }
    UNPROTECT(2);
    return p;
}


static const R_CallMethodDef CallEntries[] = {
    {"C_utf8_len", (DL_FUNC) &C_utf8_len, 1},
    {"C_utf8_codelen", (DL_FUNC) &C_utf8_codelen, 1},
    {"C_utf8_code", (DL_FUNC) &C_utf8_code, 1},
    {"C_utf8_to_utf32", (DL_FUNC) &C_utf8_to_utf32, 1},
    {NULL, NULL, 0}
};

void R_init_utftools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
