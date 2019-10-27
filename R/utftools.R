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


#' Convert a UTF-8 string to UTF-16 raw string.
#' @param text a scalar character
#' @param endian little endian or big endian?
#' @export
utf8_to_utf16 <- function(text, endian = c("little", "big")) {
    endian <- match.arg(endian)
    s <- .Call(C_utf8_to_utf16, text, endian)
    attr(s, "encoding") <- if (endian == "little") "UTF-16LE" else "UTF-16BE"
    s
}


#' Convert a UTF-16 raw string to UTF-8 string.
#' @param s a UTF-16 raw string
#' @export
utf16_to_utf8 <- function(s) {
    text <- .Call(C_utf16_to_utf8, s)
    Encoding(text) <- "UTF-8"
    text
}


#' Convert a UTF-8 string to UTF-32 raw string.
#' @param text a scalar character
#' @param endian little endian or big endian?
#' @export
utf8_to_utf32 <- function(text, endian = c("little", "big")) {
    endian <- match.arg(endian)
    s <- .Call(C_utf8_to_utf32, text, endian)
    attr(s, "encoding") <- if (endian == "little") "UTF-32LE" else "UTF-32BE"
    s
}


#' Convert a UTF-32 raw string to UTF-8 string.
#' @param s a UTF-32 raw string
#' @export
utf32_to_utf8 <- function(s) {
    text <- .Call(C_utf32_to_utf8, s)
    Encoding(text) <- "UTF-8"
    text
}
