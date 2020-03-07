test_that("utf32_to_text and text_to_utf32 work", {
    s <- "a𐐀𝜺"
    s_utf32 <- text_to_utf32(s)
    expect_equal(as.raw(s_utf32), myiconv(s, "UTF-32LE"))
    expect_equal(utf32_to_text(s_utf32), s)

    s_utf32 <- text_to_utf32(s, endian = "big")
    expect_equal(as.raw(s_utf32), myiconv(s, "UTF-32BE"))
    expect_equal(utf32_to_text(s_utf32), s)
})
