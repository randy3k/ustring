#' @docType package
#' @useDynLib ustring, .registration = TRUE
"_PACKAGE"


#' Calculate the number of code points. Fasterer version of `nchar(..., type = "chars")`.
#' @param x a scalar character or ustring
#' @export
n_code_points <- function(x) {
    if (is.character(x)) {
        return(.Call(C_ncp_text, x))
    }
    if (is.raw(x)) {
        encoding <- attr(x, "encoding")
        if (is.null(encoding) || startsWith(encoding, "UTF-8")) {
            return(.Call(C_ncp_utf8, x))
        } else if (startsWith(encoding, "UTF-16")) {
            return(.Call(C_ncp_utf16, x))
        } else if (startsWith(encoding, "UTF-32")) {
            return(.Call(C_ncp_utf32, x))
        }
    }
    stop("unsupported type")
}


#' Unicode code points
#' @param x a scalar character or ustring
#' @return an integer vector of code points
#' @export
code_points <- function(x) {
    if (is.character(x)) {
        return(.Call(C_text_to_code_points, x))
    } else if (is.raw(x)) {
        encoding <- attr(x, "encoding")
        if (is.null(encoding) || startsWith(encoding, "UTF-8")) {
            return(.Call(C_utf8_to_code_points, x))
        } else if (startsWith(encoding, "UTF-16")) {
            return(.Call(C_utf16_to_code_points, x))
        } else if (startsWith(encoding, "UTF-32")) {
            return(.Call(C_utf32_to_code_points, x))
        }
    }
    stop("unsupported type")
}

#' Convert unicode code points to text
#' @param x a vector of code points
#' @return a scalar character
#' @export
code_points_to_text <- function(x) {
    return(.Call(C_code_points_to_text, x))
}

#' Convert unicode code points to UTF-8 ustring.
#' @param x a vector of code points
#' @return a scalar character
#' @export
code_points_to_utf8 <- function(x) {
    return(.Call(C_code_points_to_utf8, x))
}

#' Convert unicode code points to UTF-16 ustring.
#' @param x a vector of code points
#' @param endian little endian or big endian?
#' @return a scalar character
#' @export
code_points_to_utf16 <- function(x, endian = "big") {
    return(.Call(C_code_points_to_utf16, x, endian))
}

#' Convert unicode code points to UTF-32 ustring.
#' @param x a vector of code points
#' @param endian little endian or big endian?
#' @return a scalar character
#' @export
code_points_to_utf32 <- function(x, endian = "big") {
    return(.Call(C_code_points_to_utf32, x, endian))
}

#' Show the encoding of a ustring
#' @param x a ustring
encoding <- function(x) {
    if (!inherits(x, "ustring")) {
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


#' Convert a UTF-8 text to UTF-8 ustring.
#' @param text a scalar character
#' @export
text_to_utf8 <- function(text) {
    .Call(C_text_to_utf8, text)
}


#' Convert a UTF-8 ustring to UTF-8 text.
#' @param s a UTF-8 ustring
#' @export
utf8_to_text <- function(s) {
    .Call(C_utf8_to_text, s)
}


#' Convert a UTF-8 text to UTF-16 ustring.
#' @param text a scalar character
#' @param endian little endian or big endian?
#' @export
text_to_utf16 <- function(text, endian = "big") {
    .Call(C_text_to_utf16, text, endian)
}


#' Convert a UTF-16 ustring to UTF-8 text.
#' @param s a UTF-16 ustring
#' @export
utf16_to_text <- function(s) {
    .Call(C_utf16_to_text, s)
}


#' Convert UTF-8 text to UTF-32 ustring.
#' @param text UTF-8 ustring
#' @param endian little endian or big endian?
#' @export
text_to_utf32 <- function(text, endian = "big") {
    .Call(C_text_to_utf32, text, endian)
}

#' Convert a UTF-32 ustring to UTF-8 text.
#' @param s a UTF-32 ustring
#' @export
utf32_to_text <- function(s) {
    .Call(C_utf32_to_text, s)
}


#' @export
#' @method print ustring
print.ustring <- function(x, ...) {
    encoding <- attr(x, "encoding")
    cat(encoding, ": ", sep = "")
    cat(x)
    cat("\n")
}


#' Convert a scaler character to ustring
#' @method as.ustring character
#' @rdname as.ustring
#' @export
#' @param x a scalar character
#' @param encoding a scalar character
#' @param ... ignored
as.ustring.character <- function(x, encoding, ...) {
    if (missing(encoding) || encoding == "UTF-8") {
        x <- charToRaw(enc2utf8(x))
        attr(x, "encoding") <- "UTF-8"
        class(x) <- "ustring"
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

#' Convert a scaler character to ustring
#' @method as.ustring ustring
#' @rdname as.ustring
#' @export
#' @param x a scalar character
#' @param encoding a scalar character
#' @param ... ignored
as.ustring.ustring <- function(x, encoding, ...) {
    if (!missing(encoding) && attr(x, "encoding") != encoding) {
        stop("incompatible encoding")
    }
    return(x)
}

#' @export
#' @rdname as.ustring
as.ustring <- function(x, ...) {
    UseMethod("as.ustring", x)
}

#' Convert a ustring to text
#' @method as.character ustring
#' @param x a ustring
#' @param ... ignored
#' @export
as.character.ustring <- function(x, ...) {
    if (inherits(x, "ustring")) {
        encoding <- attr(x, "encoding")
        if (encoding == "UTF-8") {
            utf8_to_text(x)
        } else if (startsWith(encoding, "UTF-16")) {
            utf16_to_text(x)
        } else if (startsWith(encoding, "UTF-32")) {
            utf32_to_text(x)
        }
    } else {
        stop("unsupported type")
    }
}
