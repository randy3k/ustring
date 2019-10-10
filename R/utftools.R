#' @docType package
#' @useDynLib utftools, .registration = TRUE, .fixes = "C_"
"_PACKAGE"


#' @export
utf8_decode <- function(text) {
    .Call(C_utf8_decode, text)
}
