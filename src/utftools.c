#include <string.h>
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include "utf8.h"
#include "utf32.h"


#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif

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
        if (LENGTH(s_) != 1) {
            Rf_error("expect one-element character or UTF-8 raw string");
        }
        SEXP c = Rf_asChar(s_);
        s = (const unsigned char*) R_CHAR(c);
        if (!is_ascii(s) && Rf_getCharCE(c) != CE_UTF8) {
            Rf_error("non UTF-8 encoding");
        }
    } else if (TYPEOF(s_) == RAWSXP) {
        s = RAW(s_);
        SEXP le_ = Rf_getAttrib(s_, Rf_install("encoding"));
        if (le_ != R_NilValue && strcmp(R_CHAR(Rf_asChar(le_)), "UTF-8") != 0) {
            Rf_error("non UTF-8 encoding");
        }
    } else {
        Rf_error("expect one-element character or UTF-8 raw string");
    }
    return s;
}


static const unsigned char* validate_utf32(SEXP s_, int* le) {
    const unsigned char* s;
    if (TYPEOF(s_) == RAWSXP) {
        s = RAW(s_);
    } else {
        Rf_error("expect UTF-32 raw string");
    }
    long n = LENGTH(s_);
    SEXP le_ = Rf_getAttrib(s_, Rf_install("encoding"));
    int known_endianness = 0;
    if (n == 0) {
        *le = 1;
        known_endianness = 1;
    } else if (le_ == R_NilValue) {
        if (n > 4) {
            if (s[0] == 0 && s[1] == 0 && s[2] == 0xFE && s[3] == 0xFF) {
                *le = 0;
                s += 4;
                known_endianness = 1;
            } else if (s[0] == 0xFF && s[1] == 0xFE && s[2] == 0 && s[3] == 0) {
                *le = 1;
                s += 4;
                known_endianness = 1;
            }
        }
    } else if (strcmp(R_CHAR(Rf_asChar(le_)), "UTF-32LE") == 0) {
        *le = 1;
        known_endianness = 1;
    } else if (strcmp(R_CHAR(Rf_asChar(le_)), "UTF-32BE") == 0) {
        *le = 0;
        known_endianness = 1;
    }
    if (!known_endianness) Rf_error("unknown endianness");
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


void utf8_codelen_callback(int cp, int m, void* data, long i) {
    int* pt = (int*) data;
    pt[i] = m? m : NA_INTEGER;
}

SEXP C_utf8_codelen(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    long n = utf8_len_(s_, s);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, n));
    int* pt = INTEGER(p);
    utf8_cp_collector(s, n, utf8_codelen_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}


void utf8_code_callback(int cp, int m, void* data, long i) {
    int* pt = (int*) data;
    pt[i] = cp? cp : NA_INTEGER;
}

SEXP C_utf8_code(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    long n = utf8_len_(s_, s);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, n));
    int* pt = INTEGER(p);
    utf8_cp_collector(s, n, utf8_code_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}

void utf8_to_utf32_little_callback(int cp, int m, void* data, long i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf32_decode1_little(cp, *t);
}

void utf8_to_utf32_big_callback(int cp, int m, void* data, long i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf32_decode1_big(cp, *t);;
}

SEXP C_utf8_to_utf32(SEXP s_, SEXP endian) {
    PROTECT(s_);
    char le = CHAR(Rf_asChar(endian))[0];
    const unsigned char* s = validate_utf8(s_);;
    long n = utf8_len_(s_, s);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, n * sizeof(uint32_t) / sizeof(char)));
    unsigned char* t = (unsigned char*) RAW(p);
    unsigned char* w = t;
    if (le == 'l') {
        utf8_cp_collector(s, n, utf8_to_utf32_little_callback, &w);
    } else {
        utf8_cp_collector(s, n, utf8_to_utf32_big_callback, &w);
    }
    SETLENGTH(p, w - t);
    UNPROTECT(2);
    return p;
}


void utf32_to_utf8_callback(int cp, int m, void* data, long i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf8_decode1(cp, *t);
}

SEXP C_utf32_to_utf8(SEXP s_) {
    PROTECT(s_);
    long n = LENGTH(s_);
    long m = n / 4;
    int le;
    const unsigned char* s = validate_utf32(s_, &le);
    SEXP p = PROTECT(Rf_allocVector(STRSXP, 1));
    unsigned char* t;
    t = (unsigned char*) malloc(4 * m * sizeof(char));
    unsigned char* w = t;
    utf32_cp_collector(s, m, utf32_to_utf8_callback, &w, le);
    w[0] = '\0';
    SET_STRING_ELT(p, 0, Rf_mkChar((const char*) t));
    free(t);
    UNPROTECT(2);
    return p;
}

static const R_CallMethodDef CallEntries[] = {
    {"C_utf8_len", (DL_FUNC) &C_utf8_len, 1},
    {"C_utf8_codelen", (DL_FUNC) &C_utf8_codelen, 1},
    {"C_utf8_code", (DL_FUNC) &C_utf8_code, 1},
    {"C_utf8_to_utf32", (DL_FUNC) &C_utf8_to_utf32, 2},
    {"C_utf32_to_utf8", (DL_FUNC) &C_utf32_to_utf8, 1},
    {NULL, NULL, 0}
};

void R_init_utftools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
