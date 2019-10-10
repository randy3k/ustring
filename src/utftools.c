#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include "utf8.h"

static const R_CallMethodDef CallEntries[] = {
    {"utf8_decode", (DL_FUNC) &utf8_decode, 1},
    {NULL, NULL, 0}
};

void R_init_utftools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
