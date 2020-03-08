test_that("as.ustring works for UTF-8", {
    s <- "a𐐀𝜺"
    s_utf8 <- as.ustring(s, encoding = "UTF-8")
    expect_equal(as.character(s_utf8), s)
})


test_that("as.ustring works for UTF-16", {
    s <- "a𐐀𝜺"
    s_utf16le <- as.ustring(s, encoding = "UTF-16LE")
    s_utf16be <- as.ustring(s, encoding = "UTF-16BE")
    expect_equal(as.character(s_utf16le), s)
    expect_equal(as.character(s_utf16be), s)
})


test_that("as.ustring works for UTF-32 ", {
    s <- "a𐐀𝜺"
    s_utf32le <- as.ustring(s, encoding = "UTF-32LE")
    s_utf32be <- as.ustring(s, encoding = "UTF-32BE")
    expect_equal(as.character(s_utf32le), s)
    expect_equal(as.character(s_utf32be), s)
})
