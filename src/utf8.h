#ifndef UTF8_H__
#define UTF8_H__


#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

SEXP C_utf8_len(SEXP s_);

SEXP C_utf8_cplen(SEXP s_);

SEXP C_utf8_decode(SEXP s_);



#endif /* end of include guard: UTF8_H__ */
