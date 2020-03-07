test_that("utf8_to_text and text_to_utf8 work", {
    s <- "a𐐀𝜺"
    s_utf8 <- text_to_utf8(s)
    expect_equal(as.raw(s_utf8), myiconv(s, "UTF-8"))
    expect_equal(utf8_to_text(s_utf8), s)
})
