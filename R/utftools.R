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
utf8_codelen <- function(text) {
    .Call(C_utf8_codelen, text)
}


#' Get the unicode code point for each char in a UTF-8 encoded text.
#' @param text a scalar character
#' @export
#' @export
utf8_code <- function(text) {
    .Call(C_utf8_code, text)
}

#' Convert a UTF-8 encoded text to UTF-32 bytestring.
#' @param text a scalar character
#' @param little_endian use little endian
#' @export
#' @export
utf8_to_utf32 <- function(text, little_endian = TRUE) {
    s <- .Call(C_utf8_to_utf32, text, little_endian)
    class(s) <- "bytestring"
    s
}
