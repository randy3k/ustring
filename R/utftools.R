#' @docType package
#' @useDynLib utftools, .registration = TRUE
"_PACKAGE"


#' @export
utf8_len <- function(text) {
    .Call(C_utf8_len, text)
}


#' @export
utf8_cplen <- function(text) {
    .Call(C_utf8_cplen, text)
}


#' @export
utf8_decode <- function(text) {
    .Call(C_utf8_decode, text)
}
