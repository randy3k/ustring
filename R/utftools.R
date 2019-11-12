#' @docType package
#' @useDynLib utftools, .registration = TRUE
"_PACKAGE"


#' Calculate the number of code points
#' @param x a scalar character or utfstring
#' @export
ncodept <- function(x) {
    if (is.character(x)) {
        return(.Call(C_utf8_ncodept, x))
    }
    if (is.raw(x)) {
        encoding <- attr(x, "encoding")
        if (is.null(encoding) || startsWith(encoding, "UTF-8")) {
            return(.Call(C_utf8_ncodept, x))
        } else if (startsWith(encoding, "UTF-16")) {
            return(.Call(C_utf16_ncodept, x))
        } else if (startsWith(encoding, "UTF-32")) {
            return(.Call(C_utf32_ncodept, x))
        }
    }
    stop("unsupported type")
}


#' Unicode code points
#' @param x a scalar character or utfstring
#' @export
codept <- function(x) {
    if (is.character(x)) {
        return(.Call(C_utf8_codept, x))
    } else if (is.raw(x)) {
        encoding <- attr(x, "encoding")
        if (is.null(encoding) || startsWith(encoding, "UTF-8")) {
            return(.Call(C_utf8_codept, x))
        } else if (startsWith(encoding, "UTF-16")) {
            return(.Call(C_utf16_codept, x))
        } else if (startsWith(encoding, "UTF-32")) {
            return(.Call(C_utf32_codept, x))
        }
    }
    stop("unsupported type")
}


#' Show the encoding of a utfstring
#' @param x a utfstring
encoding <- function(x) {
    if (!inherits(x, "utfstring")) {
        stop("expect utstring")
    }
    attr(x, "encoding")
}


# #' Calcuate number of bytes of each code point in a UTF-8 encoded text.
# #' @param text a scalar character
# #' @export
# code_nbytes <- function(text) {
#     .Call(C_code_nbytes, text)
# }


#' Convert a UTF-8 string to UTF-16 utfstring.
#' @param text a scalar character or UTF-8 utfstring
#' @param endian little endian or big endian?
#' @export
utf8_to_utf16 <- function(text, endian = "little") {
    s <- .Call(C_utf8_to_utf16, text, endian)
    attr(s, "encoding") <- if (endian == "little") "UTF-16LE" else "UTF-16BE"
    class(s) <- "utfstring"
    s
}


#' Convert a UTF-16 utfstring to UTF-8 utfstring.
#' @param s a UTF-16 utfstring
#' @export
utf16_to_utf8 <- function(s) {
    s <- .Call(C_utf16_to_utf8, s)
    attr(s, "encoding") <- "UTF-8"
    class(s) <- "utfstring"
    s
}

#' Convert a UTF-16 utfstring to UTF-8 text.
#' @param s a UTF-16 utfstring
#' @export
utf16_to_text <- function(s) {
    text <- .Call(C_utf16_to_text, s)
    Encoding(text) <- "UTF-8"
    text
}


#' Convert a UTF-8 string to UTF-32 utfstring.
#' @param text a scalar character or UTF-8 utfstring
#' @param endian little endian or big endian?
#' @export
utf8_to_utf32 <- function(text, endian = "little") {
    s <- .Call(C_utf8_to_utf32, text, endian)
    attr(s, "encoding") <- if (endian == "little") "UTF-32LE" else "UTF-32BE"
    class(s) <- "utfstring"
    s
}


#' Convert a UTF-32 utfstring to UTF-8 utfstring.
#' @param s a UTF-32 utfstring
#' @export
utf32_to_utf8 <- function(s) {
    s <- .Call(C_utf32_to_utf8, s)
    attr(s, "encoding") <- "UTF-8"
    class(s) <- "utfstring"
    s
}

#' Convert a UTF-32 utfstring to UTF-8 text.
#' @param s a UTF-32 utfstring
#' @export
utf32_to_text <- function(s) {
    text <- .Call(C_utf32_to_text, s)
    Encoding(text) <- "UTF-8"
    text
}


#' @export
print.utfstring <- function(x, ...) {
    encoding <- attr(x, "encoding")
    cat(encoding, ": ", sep = "")
    cat(x)
    cat("\n")
}


#' Convert a scaler character to utfstring
#' @export
#' @param x a scalar character
#' @param encoding a scalar character
as.utfstring <- function(x, encoding) {
    if (inherits(x, "utfstring")) {
        if (!missing(encoding) && attr(x, "encoding") != encoding) {
            stop("incompatible encoding")
        }
        return(x)
    } else if (is.character(x)) {
        if (missing(encoding) || encoding == "UTF-8") {
            x <- charToRaw(enc2utf8(x))
            attr(x, "encoding") <- "UTF-8"
            class(x) <- "utfstring"
            return(x)
        } else if (startsWith(encoding, "UTF-16")) {
            if (encoding == "UTF-16BE") {
                endian <- "big"
            } else if (encoding == "UTF-16LE") {
                endian <- "little"
            } else if (encoding == "UTF-16") {
                stop("require endianess: 'UTF-16BE' or 'UTF-16LE'")
            } else {
                stop("unsupported type")
            }
            return(utf8_to_utf16(x, endian = endian))
        } else if (startsWith(encoding, "UTF-32")) {
            if (encoding == "UTF-32BE") {
                endian <- "big"
            } else if (encoding == "UTF-32LE") {
                endian <- "little"
            } else if (encoding == "UTF-32") {
                stop("require endianess: 'UTF-32BE' or 'UTF-32LE'")
            } else {
                stop("unsupported type")
            }
            return(utf8_to_utf32(x, endian = endian))
        }
    }
    stop("unsupported type")
}


#' @method as.character utfstring
#' @export
as.character.utfstring <- function(x, ...) {
    if (inherits(x, "utfstring")) {
        encoding <- attr(x, "encoding")
        if (encoding == "UTF-8") {
            text <- rawToChar(x)
            Encoding(text) <- "UTF-8"
            text
        } else if (startsWith(encoding, "UTF-16")) {
            utf16_to_text(x)
        } else if (startsWith(encoding, "UTF-32")) {
            utf32_to_text(x)
        }
    } else {
        stop("unsupported type")
    }
}
