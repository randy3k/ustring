test_that("as.utfstring works for UTF-8", {
    s <- "a𐐀𝜺"
    s_utf8 <- as.utfstring(s)
    expect_equal(s_utf8, utf16_to_utf8(utf8_to_utf16(s)))
})



test_that("as.utfstring works for UTF-16", {
    s <- "a𐐀𝜺"
    s_utf16le <- as.utfstring(s, encoding = "UTF-16LE")
    s_utf16be <- as.utfstring(s, encoding = "UTF-16BE")
    expect_s3_class(s_utf16le, "utfstring")
    expect_s3_class(s_utf16be, "utfstring")
    expect_equal(s_utf16le, utf8_to_utf16(s, endian = "little"))
    expect_equal(s_utf16be, utf8_to_utf16(s, endian = "big"))
    expect_equal(utf16_to_text(s_utf16le), s)
    expect_equal(utf16_to_text(s_utf16be), s)
})


test_that("as.utfstring works for UTF-32 ", {
    s <- "a𐐀𝜺"
    s_utf32le <- as.utfstring(s, encoding = "UTF-32LE")
    s_utf32be <- as.utfstring(s, encoding = "UTF-32BE")
    expect_s3_class(s_utf32le, "utfstring")
    expect_s3_class(s_utf32be, "utfstring")
    expect_equal(s_utf32le, utf8_to_utf32(s, endian = "little"))
    expect_equal(s_utf32be, utf8_to_utf32(s, endian = "big"))
    expect_equal(utf32_to_text(s_utf32le), s)
    expect_equal(utf32_to_text(s_utf32be), s)
})
