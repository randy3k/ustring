#' @docType package
#' @useDynLib utftools, .registration = TRUE
"_PACKAGE"


#' Calculate the number of code points
#' @param x a scalar character or utfstring
#' @export
npt <- function(x) {
    if (is.character(x)) {
        return(.Call(C_text_npt, x))
    }
    if (is.raw(x)) {
        encoding <- attr(x, "encoding")
        if (is.null(encoding) || startsWith(encoding, "UTF-8")) {
            return(.Call(C_utf8_npt, x))
        } else if (startsWith(encoding, "UTF-16")) {
            return(.Call(C_utf16_npt, x))
        } else if (startsWith(encoding, "UTF-32")) {
            return(.Call(C_utf32_npt, x))
        }
    }
    stop("unsupported type")
}


#' Unicode code points
#' @param x a scalar character or utfstring
#' @export
codept <- function(x) {
    if (is.character(x)) {
        return(.Call(C_text_codept, x))
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


#' Convert a UTF-8 text to UTF-8 utfstring.
#' @param text a scalar character
#' @export
text_to_utf8 <- function(text) {
    s <- charToRaw(text)
    attr(s, "encoding") <- "UTF-8"
    class(s) <- "utfstring"
    s
}

#' Convert a UTF-8 text to UTF-8 utfstring.
#' @param s a UTF-8 utfstring
#' @export
utf8_to_text <- function(s) {
    text <- rawToChar(s)
    Encoding(text) <- "UTF-8"
    text
}

#' Convert a UTF-8 text to UTF-16 utfstring.
#' @param text a scalar character
#' @param endian little endian or big endian?
#' @export
text_to_utf16 <- function(text, endian = "little") {
    s <- .Call(C_text_to_utf16, text, endian)
    attr(s, "encoding") <- if (endian == "little") "UTF-16LE" else "UTF-16BE"
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


#' Convert UTF-8 text to UTF-32 utfstring.
#' @param text UTF-8 utfstring
#' @param endian little endian or big endian?
#' @export
text_to_utf32 <- function(text, endian = "little") {
    s <- .Call(C_text_to_utf32, text, endian)
    attr(s, "encoding") <- if (endian == "little") "UTF-32LE" else "UTF-32BE"
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
print.utfstring <- function(s, ...) {
    encoding <- attr(s, "encoding")
    cat(encoding, ": ", sep = "")
    cat(s)
    cat("\n")
}


#' Convert a scaler character to utfstring
#' @method as.utfstring character
#' @export
#' @param x a scalar character
#' @param encoding a scalar character
as.utfstring.character <- function(x, encoding) {
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
        return(text_to_utf16(x, endian = endian))
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
        return(text_to_utf32(x, endian = endian))
    }
}

#' Convert a scaler character to utfstring
#' @method as.utfstring utfstring
#' @export
#' @param x a scalar character
#' @param encoding a scalar character
as.utfstring.utfstring <- function(x, encoding) {
    if (!missing(encoding) && attr(x, "encoding") != encoding) {
        stop("incompatible encoding")
    }
    return(x)
}

#' @export
as.utfstring <- function(x, ...) {
    UseMethod("as.utfstring", x)
}

#' Convert a utfstring to text
#' @method as.character utfstring
#' @param x a utfstring
#' @export
as.character.utfstring <- function(s, ...) {
    if (inherits(s, "utfstring")) {
        encoding <- attr(s, "encoding")
        if (encoding == "UTF-8") {
            utf8_to_text(s)
        } else if (startsWith(encoding, "UTF-16")) {
            utf16_to_text(s)
        } else if (startsWith(encoding, "UTF-32")) {
            utf32_to_text(s)
        }
    } else {
        stop("unsupported type")
    }
}
