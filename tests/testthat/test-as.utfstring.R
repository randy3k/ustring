test_that("as.ustring works for UTF-8", {
    s <- "a𐐀𝜺"
    s_utf8 <- as.ustring(s, encoding = "UTF-8")
    expect_s3_class(s_utf8, "ustring")
    expect_equal(s_utf8, text_to_utf8(s))
    expect_equal(utf8_to_text(s_utf8), s)
})


test_that("as.ustring works for UTF-16", {
    s <- "a𐐀𝜺"
    s_utf16le <- as.ustring(s, encoding = "UTF-16LE")
    s_utf16be <- as.ustring(s, encoding = "UTF-16BE")
    expect_s3_class(s_utf16le, "ustring")
    expect_s3_class(s_utf16be, "ustring")
    expect_equal(s_utf16le, text_to_utf16(s, endian = "little"))
    expect_equal(s_utf16be, text_to_utf16(s, endian = "big"))
    expect_equal(utf16_to_text(s_utf16le), s)
    expect_equal(utf16_to_text(s_utf16be), s)
})


test_that("as.ustring works for UTF-32 ", {
    s <- "a𐐀𝜺"
    s_utf32le <- as.ustring(s, encoding = "UTF-32LE")
    s_utf32be <- as.ustring(s, encoding = "UTF-32BE")
    expect_s3_class(s_utf32le, "ustring")
    expect_s3_class(s_utf32be, "ustring")
    expect_equal(s_utf32le, text_to_utf32(s, endian = "little"))
    expect_equal(s_utf32be, text_to_utf32(s, endian = "big"))
    expect_equal(utf32_to_text(s_utf32le), s)
    expect_equal(utf32_to_text(s_utf32be), s)
})
