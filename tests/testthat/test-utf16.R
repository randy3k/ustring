test_that("utf16 works", {
    s <- "a𐐀𝜺"
    s_utf16 <- utf8_to_utf16(s)
    expect_equal(ncodept(s_utf16), 3)
    expect_equal(codept(s_utf16), c(97L, 66560L, 120634L))
})


test_that("utf16_to_text works", {
    s <- "a𐐀𠍲"
    s_utf16 <- utf8_to_utf16(s)
    expect_equal(utf16_to_text(s_utf16), s)

    s_utf16 <- utf8_to_utf16(s, endian = "big")
    expect_equal(utf16_to_text(s_utf16), s)

    s_utf16 <- iconv(s, from = "UTF-8", to = "UTF-16", toRaw = TRUE)[[1]]
    expect_equal(utf16_to_text(s_utf16), s)
})


test_that("utf16_to_utf8 works", {
    s <- "a𐐀𠍲"
    s_utf16 <- utf8_to_utf16(s)
    expect_equal(utf16_to_utf8(s_utf16), as.utfstring(s))

    s_utf16 <- utf8_to_utf16(s, endian = "big")
    expect_equal(utf16_to_utf8(s_utf16), as.utfstring(s))

    s_utf16 <- iconv(s, from = "UTF-8", to = "UTF-16", toRaw = TRUE)[[1]]
    expect_equal(utf16_to_utf8(s_utf16), as.utfstring(s))
})
