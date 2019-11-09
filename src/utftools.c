#include <string.h>
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include "nbytes.h"
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


SEXP C_utf8_ncodept(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);
    size_t k = utf8_ncodept(s, TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_));
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}


SEXP C_utf16_ncodept(SEXP s_) {
    PROTECT(s_);
    size_t k;
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - bom * 2;
    if (le) {
        k = utf16_ncodept_little(s, n);
    } else {
        k = utf16_ncodept_big(s, n);
    }
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}


SEXP C_utf32_ncodept(SEXP s_) {
    PROTECT(s_);
    size_t k;
    int le;
    int bom = 0;
    validate_utf32(s_, &bom, &le);
    k = LENGTH(s_)/4 - bom;
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}


void codept_callback(uint32_t cp, void* data, size_t i) {
    int* pt = (int*) data;
    pt[i] = cp == (uint32_t) -1 ? NA_INTEGER : cp;
}

SEXP C_utf8_codept(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    size_t n = TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_);
    size_t k = utf8_ncodept(s, n);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf8_cp_collector(s, n, codept_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}

SEXP C_utf16_codept(SEXP s_) {
    PROTECT(s_);
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - 4* bom;
    size_t k;
    if (le) {
        k = utf16_ncodept_little(s, n);
    } else {
        k = utf16_ncodept_big(s, n);
    }
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf16_cp_collector(s, n, codept_callback, (void*) pt, le);
    UNPROTECT(2);
    return p;
}


SEXP C_utf32_codept(SEXP s_) {
    PROTECT(s_);
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf32(s_, &bom, &le);
    size_t n = LENGTH(s_) - 4* bom;
    size_t k = n/4;
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf32_cp_collector(s, n, codept_callback, (void*) pt, le);
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
    utfsize_t k = utf8_nbytes(s, n);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, k.utf16));
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
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - bom * 2;
    utfsize_t k = utf16_nbytes(s, n, le);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, k.utf8));
    unsigned char* t = (unsigned char*) RAW(p);
    unsigned char* w = t;
    utf16_cp_collector(s, n, utf16_to_utf8_callback, &w, le);
    SETLENGTH(p, w - t);
    UNPROTECT(2);
    return p;
}

SEXP C_utf16_to_text(SEXP s_) {
    PROTECT(s_);
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - bom * 2;
    utfsize_t k = utf16_nbytes(s, n, le);
    SEXP p = PROTECT(Rf_allocVector(STRSXP, 1));
    unsigned char* t;
    t = (unsigned char*) malloc(k.utf8 * sizeof(char));
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
    size_t k = utf8_ncodept(s, n);
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
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf32(s_, &bom, &le);
    size_t n = LENGTH(s_) - bom * 2;
    utfsize_t k = utf16_nbytes(s, n, le);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, k.utf8));
    unsigned char* t = (unsigned char*) RAW(p);
    unsigned char* w = t;
    utf32_cp_collector(s, n, utf32_to_utf8_callback, &w, le);
    SETLENGTH(p, w - t);
    UNPROTECT(2);
    return p;
}


SEXP C_utf32_to_text(SEXP s_) {
    PROTECT(s_);
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf32(s_, &bom, &le);
    size_t n = LENGTH(s_) - 4*bom;
    utfsize_t k = utf32_nbytes(s, n, le);
    SEXP p = PROTECT(Rf_allocVector(STRSXP, 1));
    unsigned char* t;
    t = (unsigned char*) malloc(k.utf8 * sizeof(char));
    unsigned char* w = t;
    utf32_cp_collector(s, n, utf32_to_utf8_callback, &w, le);
    w[0] = '\0';
    SET_STRING_ELT(p, 0, Rf_mkChar((const char*) t));
    free(t);
    UNPROTECT(2);
    return p;
}


// void code_nbytes_callback(uint32_t cp, void* data, size_t i) {
//     SEXP p = (SEXP) data;
//     int m;
//     m = utf8_codelen(cp);
//     INTEGER(VECTOR_ELT(p, 0))[i] = m ? m : 1;;
//     m = utf16_codelen(cp);
//     INTEGER(VECTOR_ELT(p, 1))[i] = m ? m : 2;;
//     m = utf32_codelen(cp);
//     INTEGER(VECTOR_ELT(p, 2))[i] = m ? m : 4;;
// }

// SEXP C_code_nbytes(SEXP s_) {
//     PROTECT(s_);
//     const unsigned char* s = validate_utf8(s_);;
//     size_t n = TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_);
//     size_t k = utf8_ncodept(s, n);
//     SEXP p = PROTECT(Rf_allocVector(VECSXP, 3));
//     SEXP names = PROTECT(Rf_allocVector(STRSXP, 3));
//     SET_STRING_ELT(names, 0, Rf_mkChar("utf8"));
//     SET_STRING_ELT(names, 1, Rf_mkChar("utf16"));
//     SET_STRING_ELT(names, 2, Rf_mkChar("utf32"));
//     Rf_setAttrib(p, R_NamesSymbol, names);
//     SEXP p8 = PROTECT(Rf_allocVector(INTSXP, k));
//     SEXP p16 = PROTECT(Rf_allocVector(INTSXP, k));
//     SEXP p32 = PROTECT(Rf_allocVector(INTSXP, k));
//     SET_VECTOR_ELT(p, 0, p8);
//     SET_VECTOR_ELT(p, 1, p16);
//     SET_VECTOR_ELT(p, 2, p32);
//     utf8_cp_collector(s, n, code_nbytes_callback, (void*) p);
//     UNPROTECT(6);
//     return p;
// }


static const R_CallMethodDef CallEntries[] = {
    {"C_utf8_ncodept", (DL_FUNC) &C_utf8_ncodept, 1},
    {"C_utf16_ncodept", (DL_FUNC) &C_utf16_ncodept, 1},
    {"C_utf32_ncodept", (DL_FUNC) &C_utf32_ncodept, 1},
    {"C_utf8_codept", (DL_FUNC) &C_utf8_codept, 1},
    {"C_utf16_codept", (DL_FUNC) &C_utf16_codept, 1},
    {"C_utf32_codept", (DL_FUNC) &C_utf32_codept, 1},
    {"C_utf8_to_utf16", (DL_FUNC) &C_utf8_to_utf16, 2},
    {"C_utf16_to_utf8", (DL_FUNC) &C_utf16_to_utf8, 1},
    {"C_utf16_to_text", (DL_FUNC) &C_utf16_to_text, 1},
    {"C_utf8_to_utf32", (DL_FUNC) &C_utf8_to_utf32, 2},
    {"C_utf32_to_utf8", (DL_FUNC) &C_utf32_to_utf8, 1},
    {"C_utf32_to_text", (DL_FUNC) &C_utf32_to_text, 1},
    // {"C_code_nbytes", (DL_FUNC) &C_code_nbytes, 1},
    {NULL, NULL, 0}
};

void R_init_utftools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
