#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include "utf8.h"

static const R_CallMethodDef CallEntries[] = {
    {"C_utf8_len", (DL_FUNC) &C_utf8_len, 1},
    {"C_utf8_cplen", (DL_FUNC) &C_utf8_cplen, 1},
    {"C_utf8_decode", (DL_FUNC) &C_utf8_decode, 1},
    {NULL, NULL, 0}
};

void R_init_utftools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
