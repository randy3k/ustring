myiconv <- function(s, encoding) {
    iconv(s, from = "UTF-8", to = encoding, toRaw = TRUE)[[1]]
}
