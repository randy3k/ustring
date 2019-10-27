#include <string.h>
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include "utf8.h"
#include "utf16.h"
#include "utf32.h"


#if !defined(static_inline)
#if defined(_MSC_VER) || defined(__GNUC__)
#define static_inline static __inline
#else
#define static_inline static
#endif
#endif

static int is_ascii(const unsigned char* s) {
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
    size_t n = LENGTH(s_);
    if (TYPEOF(s_) == STRSXP ) {
        if (n != 1) {
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

static const unsigned char* validate_utf16(SEXP s_, int* bom, int* le) {
    size_t n = LENGTH(s_);
    const unsigned char* s;
    if (TYPEOF(s_) == RAWSXP) {
        s = RAW(s_);
    } else {
        Rf_error("expect UTF-16 raw string");
    }
    SEXP le_ = Rf_getAttrib(s_, Rf_install("encoding"));
    int known_endianness = 0;
    if (n == 0) {
        *le = 1;
        known_endianness = 1;
    } else if (le_ == R_NilValue) {
        if (n > 4) {
            if (s[0] == 0xFE && s[1] == 0xFF) {
                *le = 0;
                s += 2;
                *bom = 1;
                known_endianness = 1;
            } else if (s[0] == 0xFF && s[1] == 0xFE) {
                *le = 1;
                s += 2;
                *bom = 1;
                known_endianness = 1;
            }
        }
    } else if (strcmp(R_CHAR(Rf_asChar(le_)), "UTF-16LE") == 0) {
        *le = 1;
        known_endianness = 1;
    } else if (strcmp(R_CHAR(Rf_asChar(le_)), "UTF-16BE") == 0) {
        *le = 0;
        known_endianness = 1;
    } else {
        Rf_error("expect UTF-16 raw string");
    }
    if (!known_endianness) Rf_error("unknown endianness");
    return s;
}


static const unsigned char* validate_utf32(SEXP s_, int* bom, int* le) {
    const unsigned char* s;
    size_t n = LENGTH(s_);
    if (TYPEOF(s_) == RAWSXP) {
        s = RAW(s_);
    } else {
        Rf_error("expect UTF-32 raw string");
    }
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
    } else {
        Rf_error("expect UTF-32 raw string");
    }
    if (!known_endianness) Rf_error("unknown endianness");
    return s;
}


SEXP C_utf8_length(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);
    size_t k = utf8_length(s, TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_));
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}

SEXP C_utf16_length(SEXP s_) {
    PROTECT(s_);
    size_t k;
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - bom * 2;
    if (le) {
        k = utf16_length_little(s, n);
    } else {
        k = utf16_length_big(s, n);
    }
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}

SEXP C_utf32_length(SEXP s_) {
    PROTECT(s_);
    size_t k;
    int le;
    int bom = 0;
    validate_utf32(s_, &bom, &le);
    k = LENGTH(s_)/4 - bom;
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}


void utf8_codelen_callback(uint32_t cp, void* data, size_t i) {
    int* pt = (int*) data;
    int m = utf8_codelen(cp);
    pt[i] = m ? m : 1;
}

SEXP C_utf8_codelen(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    size_t n = TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_);
    size_t k = utf8_length(s, n);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf8_cp_collector(s, n, utf8_codelen_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}


void utf8_code_callback(uint32_t cp, void* data, size_t i) {
    int* pt = (int*) data;
    pt[i] = cp == (uint32_t) -1 ? NA_INTEGER : cp;
}

SEXP C_utf8_code(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    size_t n = TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_);
    size_t k = utf8_length(s, n);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf8_cp_collector(s, n, utf8_code_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}

void utf8_to_utf16_little_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf16_decode1_little(cp, *t);
}

void utf8_to_utf16_big_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf16_decode1_big(cp, *t);;
}

SEXP C_utf8_to_utf16(SEXP s_, SEXP endian) {
    PROTECT(s_);
    char le = CHAR(Rf_asChar(endian))[0];
    const unsigned char* s = validate_utf8(s_);
    size_t n = TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_);
    size_t k = utf8_length(s, n);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, 4 * k));
    unsigned char* t = (unsigned char*) RAW(p);
    unsigned char* w = t;
    if (le == 'b') {
        utf8_cp_collector(s, n, utf8_to_utf16_big_callback, &w);
    } else {
        utf8_cp_collector(s, n, utf8_to_utf16_little_callback, &w);
    }
    SETLENGTH(p, w - t);
    UNPROTECT(2);
    return p;
}


void utf16_to_utf8_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    int m = utf8_decode1(cp, *t);
    *t = *t + m;
}

SEXP C_utf16_to_utf8(SEXP s_) {
    PROTECT(s_);
    size_t k = 0;
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - bom * 2;
    if (le) {
        k = utf16_length_little(s, n);
    } else {
        k = utf16_length_big(s, n);
    }
    SEXP p = PROTECT(Rf_allocVector(STRSXP, 1));
    unsigned char* t;
    t = (unsigned char*) malloc(4 * k * sizeof(char));
    unsigned char* w = t;
    utf16_cp_collector(s, n, utf16_to_utf8_callback, &w, le);
    w[0] = '\0';
    SET_STRING_ELT(p, 0, Rf_mkChar((const char*) t));
    free(t);
    UNPROTECT(2);
    return p;
}

void utf8_to_utf32_little_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf32_decode1_little(cp, *t);
}

void utf8_to_utf32_big_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf32_decode1_big(cp, *t);;
}

SEXP C_utf8_to_utf32(SEXP s_, SEXP endian) {
    PROTECT(s_);
    char le = CHAR(Rf_asChar(endian))[0];
    const unsigned char* s = validate_utf8(s_);;
    size_t n = TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_);
    size_t k = utf8_length(s, n);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, 4 * k));
    unsigned char* t = (unsigned char*) RAW(p);
    unsigned char* w = t;
    if (le == 'b') {
        utf8_cp_collector(s, n, utf8_to_utf32_big_callback, &w);
    } else {
        utf8_cp_collector(s, n, utf8_to_utf32_little_callback, &w);
    }
    SETLENGTH(p, w - t);
    UNPROTECT(2);
    return p;
}


void utf32_to_utf8_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf8_decode1(cp, *t);
}

SEXP C_utf32_to_utf8(SEXP s_) {
    PROTECT(s_);
    size_t k = 0;
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf32(s_, &bom, &le);
    size_t n = LENGTH(s_) - 4*bom;
    k = LENGTH(s_)/4 - bom;
    SEXP p = PROTECT(Rf_allocVector(STRSXP, 1));
    unsigned char* t;
    t = (unsigned char*) malloc(4 * k * sizeof(char));
    unsigned char* w = t;
    utf32_cp_collector(s, n, utf32_to_utf8_callback, &w, le);
    w[0] = '\0';
    SET_STRING_ELT(p, 0, Rf_mkChar((const char*) t));
    free(t);
    UNPROTECT(2);
    return p;
}

static const R_CallMethodDef CallEntries[] = {
    {"C_utf8_length", (DL_FUNC) &C_utf8_length, 1},
    {"C_utf16_length", (DL_FUNC) &C_utf16_length, 1},
    {"C_utf32_length", (DL_FUNC) &C_utf32_length, 1},
    {"C_utf8_codelen", (DL_FUNC) &C_utf8_codelen, 1},
    {"C_utf8_code", (DL_FUNC) &C_utf8_code, 1},
    {"C_utf8_to_utf16", (DL_FUNC) &C_utf8_to_utf16, 2},
    {"C_utf16_to_utf8", (DL_FUNC) &C_utf16_to_utf8, 1},
    {"C_utf8_to_utf32", (DL_FUNC) &C_utf8_to_utf32, 2},
    {"C_utf32_to_utf8", (DL_FUNC) &C_utf32_to_utf8, 1},
    {NULL, NULL, 0}
};

void R_init_utftools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
