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

static const unsigned char* validate_text(SEXP s_) {
    const unsigned char* s;
    if (TYPEOF(s_) == STRSXP ) {
        size_t n = LENGTH(s_);
        if (n != 1) {
            Rf_error("expect one-element character");
        }
        SEXP c = Rf_asChar(s_);
        s = (const unsigned char*) R_CHAR(c);
        if (!is_ascii(s) && Rf_getCharCE(c) != CE_UTF8) {
            Rf_error("non UTF-8 encoding");
        }
    } else {
        Rf_error("expect one-element character");
    }
    return s;
}

static const unsigned char* validate_utf8(SEXP s_) {
    const unsigned char* s;
    if (TYPEOF(s_) == RAWSXP) {
        s = RAW(s_);
        SEXP le_ = Rf_getAttrib(s_, Rf_install("encoding"));
        if (le_ != R_NilValue && strcmp(R_CHAR(Rf_asChar(le_)), "UTF-8") != 0) {
            Rf_error("non UTF-8 encoding");
        }
    } else {
        Rf_error("expect UTF-8 ustring");
    }
    return s;
}

static const unsigned char* validate_utf16(SEXP s_, int* bom, int* le) {
    size_t n = LENGTH(s_);
    const unsigned char* s;
    if (TYPEOF(s_) == RAWSXP) {
        s = RAW(s_);
    } else {
        Rf_error("expect UTF-16 ustring");
    }
    SEXP le_ = Rf_getAttrib(s_, Rf_install("encoding"));
    int known_endianness = 0;
    if (n == 0) {
        *le = 1;
        known_endianness = 1;
    } else if (le_ == R_NilValue) {
        if (n > 2) {
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
        Rf_error("expect UTF-16 ustring");
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
        Rf_error("expect UTF-32 ustring");
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
                *bom = 1;
                known_endianness = 1;
            } else if (s[0] == 0xFF && s[1] == 0xFE && s[2] == 0 && s[3] == 0) {
                *le = 1;
                s += 4;
                *bom = 1;
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
        Rf_error("expect UTF-32 ustring");
    }
    if (!known_endianness) Rf_error("unknown endianness");
    return s;
}


SEXP C_ncp_text(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_text(s_);
    size_t k = utf8_ncp(s, strlen((const char*) s));
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}

SEXP C_ncp_utf8(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);
    size_t k = utf8_ncp(s, LENGTH(s_));
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}


SEXP C_ncp_utf16(SEXP s_) {
    PROTECT(s_);
    size_t k;
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - bom * 2;
    if (le) {
        k = utf16_ncp_little(s, n);
    } else {
        k = utf16_ncp_big(s, n);
    }
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}


SEXP C_ncp_utf32(SEXP s_) {
    PROTECT(s_);
    size_t k;
    int le;
    int bom = 0;
    validate_utf32(s_, &bom, &le);
    k = LENGTH(s_)/4 - bom;
    UNPROTECT(1);
    return Rf_ScalarReal((double) k);
}


void code_points_callback(uint32_t cp, void* data, size_t i) {
    int* pt = (int*) data;
    pt[i] = cp == (uint32_t) -1 ? NA_INTEGER : cp;
}


SEXP C_code_points_text(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_text(s_);;
    size_t n =strlen((const char*) s);
    size_t k = utf8_ncp(s, n);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf8_cp_collector(s, n, code_points_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}

SEXP C_code_points_utf8(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_utf8(s_);;
    size_t n = LENGTH(s_);
    size_t k = utf8_ncp(s, n);
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf8_cp_collector(s, n, code_points_callback, (void*) pt);
    UNPROTECT(2);
    return p;
}

SEXP C_code_points_utf16(SEXP s_) {
    PROTECT(s_);
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf16(s_, &bom, &le);
    size_t n = LENGTH(s_) - 4* bom;
    size_t k;
    if (le) {
        k = utf16_ncp_little(s, n);
    } else {
        k = utf16_ncp_big(s, n);
    }
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf16_cp_collector(s, n, code_points_callback, (void*) pt, le);
    UNPROTECT(2);
    return p;
}


SEXP C_code_points_utf32(SEXP s_) {
    PROTECT(s_);
    int le;
    int bom = 0;
    const unsigned char* s = validate_utf32(s_, &bom, &le);
    size_t n = LENGTH(s_) - 4* bom;
    size_t k = n/4;
    SEXP p = PROTECT(Rf_allocVector(INTSXP, k));
    int* pt = INTEGER(p);
    utf32_cp_collector(s, n, code_points_callback, (void*) pt, le);
    UNPROTECT(2);
    return p;
}


SEXP C_text_to_utf8(SEXP s_) {
    PROTECT(s_);
    const unsigned char* s = validate_text(s_);
    size_t n = strlen((const char*) s);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, n));
    memcpy(RAW(p), s, n);
    Rf_setAttrib(p, R_ClassSymbol, Rf_mkString("ustring"));
    SEXP enc = PROTECT(Rf_mkString("UTF-8"));
    Rf_setAttrib(p, Rf_install("encoding"), enc);
    UNPROTECT(3);
    return p;
}


SEXP C_utf8_to_text(SEXP s_) {
    PROTECT(s_);
    size_t n = LENGTH(s_);
    SEXP p = PROTECT(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(p, 0, Rf_mkCharLenCE((const char*) RAW(s_), n, CE_UTF8));
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


SEXP C_text_to_utf16(SEXP s_, SEXP endian) {
    PROTECT(s_);
    char le = CHAR(Rf_asChar(endian))[0];
    const unsigned char* s = validate_text(s_);
    size_t n = strlen((const char*) s);
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
    Rf_setAttrib(p, R_ClassSymbol, Rf_mkString("ustring"));
    SEXP enc = PROTECT(Rf_mkString(le == 'b' ? "UTF-16BE" : "UTF-16LE"));
    Rf_setAttrib(p, Rf_install("encoding"), enc);
    UNPROTECT(3);
    return p;
}


void utf16_to_utf8_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    int m = utf8_decode1(cp, *t);
    *t = *t + m;
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
    t = (unsigned char*) malloc(k.utf8 * sizeof(char) + 1);
    unsigned char* w = t;
    utf16_cp_collector(s, n, utf16_to_utf8_callback, &w, le);
    w[0] = '\0';
    SET_STRING_ELT(p, 0, Rf_mkCharCE((const char*) t, CE_UTF8));
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


SEXP C_text_to_utf32(SEXP s_, SEXP endian) {
    PROTECT(s_);
    char le = CHAR(Rf_asChar(endian))[0];
    const unsigned char* s = validate_text(s_);;
    size_t n = TYPEOF(s_) == STRSXP ? strlen((const char*) s) : LENGTH(s_);
    size_t k = utf8_ncp(s, n);
    SEXP p = PROTECT(Rf_allocVector(RAWSXP, 4 * k));
    unsigned char* t = (unsigned char*) RAW(p);
    unsigned char* w = t;
    if (le == 'b') {
        utf8_cp_collector(s, n, utf8_to_utf32_big_callback, &w);
    } else {
        utf8_cp_collector(s, n, utf8_to_utf32_little_callback, &w);
    }
    SETLENGTH(p, w - t);
    Rf_setAttrib(p, R_ClassSymbol, Rf_mkString("ustring"));
    SEXP enc = PROTECT(Rf_mkString(le == 'b' ? "UTF-32BE" : "UTF-32LE"));
    Rf_setAttrib(p, Rf_install("encoding"), enc);
    UNPROTECT(3);
    return p;
}


void utf32_to_utf8_callback(uint32_t cp, void* data, size_t i) {
    unsigned char** t = (unsigned char**) data;
    *t = *t + utf8_decode1(cp, *t);
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
    t = (unsigned char*) malloc(k.utf8 * sizeof(char) + 1);
    unsigned char* w = t;
    utf32_cp_collector(s, n, utf32_to_utf8_callback, &w, le);
    w[0] = '\0';
    SET_STRING_ELT(p, 0, Rf_mkCharCE((const char*) t, CE_UTF8));
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
//     size_t k = utf8_ncp(s, n);
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
    {"C_ncp_text", (DL_FUNC) &C_ncp_text, 1},
    {"C_ncp_utf8", (DL_FUNC) &C_ncp_utf8, 1},
    {"C_ncp_utf16", (DL_FUNC) &C_ncp_utf16, 1},
    {"C_ncp_utf32", (DL_FUNC) &C_ncp_utf32, 1},
    {"C_code_points_text", (DL_FUNC) &C_code_points_text, 1},
    {"C_code_points_utf8", (DL_FUNC) &C_code_points_utf8, 1},
    {"C_code_points_utf16", (DL_FUNC) &C_code_points_utf16, 1},
    {"C_code_points_utf32", (DL_FUNC) &C_code_points_utf32, 1},
    {"C_text_to_utf8", (DL_FUNC) &C_text_to_utf8, 1},
    {"C_utf8_to_text", (DL_FUNC) &C_utf8_to_text, 1},
    {"C_text_to_utf16", (DL_FUNC) &C_text_to_utf16, 2},
    {"C_utf16_to_text", (DL_FUNC) &C_utf16_to_text, 1},
    {"C_text_to_utf32", (DL_FUNC) &C_text_to_utf32, 2},
    {"C_utf32_to_text", (DL_FUNC) &C_utf32_to_text, 1},
    // {"C_code_nbytes", (DL_FUNC) &C_code_nbytes, 1},
    {NULL, NULL, 0}
};

void R_init_ustring(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
