test_that("utf16_to_text and text_to_utf16 work", {
    s <- "a𐐀𝜺"
    s_utf16 <- text_to_utf16(s)
    expect_equal(as.raw(s_utf16), myiconv(s, "UTF-16BE"))
    expect_equal(utf16_to_text(s_utf16), s)

    s_utf16 <- text_to_utf16(s, endian = "little")
    expect_equal(as.raw(s_utf16), myiconv(s, "UTF-16LE"))
    expect_equal(utf16_to_text(s_utf16), s)
})
