#' @docType package
#' @useDynLib utftools, .registration = TRUE
"_PACKAGE"


#' Calcuate the number of code points of a UTF-8 encoded text.
#' @param text a scalar character
#' @export
utf8_len <- function(text) {
    .Call(C_utf8_len, text)
}


#' Calcuate code point lenghts of each char in a UTF-8 encoded text.
#' @param text a scalar character
#' @export
utf8_cplen <- function(text) {
    .Call(C_utf8_cplen, text)
}


#' Get the unicode code point for each char in a UTF-8 encoded text.
#' @param text a scalar character
#' @export
#' @export
utf8_decode <- function(text) {
    .Call(C_utf8_decode, text)
}
