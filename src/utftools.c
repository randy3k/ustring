#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>


typedef struct {
  unsigned char mask;  // data stored in these bits
  unsigned char lead;  // leading bits
  int trailing;  // number of bits used to store data
} utf8_mask;


utf8_mask utf8_masks[] = {
  {0b01111111, 0b00000000},
  {0b00011111, 0b11000000},
  {0b00001111, 0b11100000},
  {0b00000111, 0b11110000}
};

size_t UTF8_MASKS_LEN = sizeof(utf8_masks) / sizeof(utf8_masks[0]);


inline int utf8_is_continuation(const unsigned char c) {
  return (c >> 6) == 0b00000010;
}


int utf8_cplen(const unsigned char c) {
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

// [[Rcpp::export]]
int utf8_ncp(const char* s) {
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

int utf8_cp1(const char* s, uint32_t* cp) {
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
    temp = (temp << 6) + (*c & 0b00111111);
  }
  *cp = temp;
  return m;
}

// [[Rcpp::export]]
SEXP utf8_cp(SEXP s_) {
  PROTECT(s_);
  const char* s = R_CHAR(Rf_asChar(s_));
  int n = utf8_ncp(s);
  SEXP p = PROTECT(Rf_allocVector(INTSXP, n));
  int* pt = INTEGER(p);
  uint32_t cp;
  int m;
  const char* t;
  int i;
  t = s;
  i = 0;
  while (i < n) {
    m = utf8_cp1(t, &cp);
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
