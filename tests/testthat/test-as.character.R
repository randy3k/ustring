test_that("as.character works for UTF-8", {
    s <- "a𐐀𝜺"
    s_utf8 <- as.utfstring(s)
    expect_equal(as.character(s_utf8), s)
})



test_that("as.utfstring works for UTF-16", {
    s <- "a𐐀𝜺"
    s_utf16le <- as.utfstring(s, encoding = "UTF-16LE")
    s_utf16be <- as.utfstring(s, encoding = "UTF-16BE")
    expect_equal(as.character(s_utf16le), s)
    expect_equal(as.character(s_utf16be), s)
})


test_that("as.utfstring works for UTF-32 ", {
    s <- "a𐐀𝜺"
    s_utf32le <- as.utfstring(s, encoding = "UTF-32LE")
    s_utf32be <- as.utfstring(s, encoding = "UTF-32BE")
    expect_equal(as.character(s_utf32le), s)
    expect_equal(as.character(s_utf32be), s)
})
