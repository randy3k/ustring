test_that("utf32 works", {
    s <- "a𐐀𝜺"
    s_utf32 <- utf8_to_utf32(s)
    expect_equal(ncodept(s_utf32), 3)
    expect_equal(codept(s_utf32), c(97L, 66560L, 120634L))
})


test_that("utf32_to_text works", {
    s <- "a𐐀𝜺"
    s_utf32 <- utf8_to_utf32(s)
    expect_equal(utf32_to_text(s_utf32), s)

    s_utf32 <- utf8_to_utf32(s, endian = "big")
    expect_equal(utf32_to_text(s_utf32), s)

    s_utf32 <- iconv(s, from = "UTF-8", to = "UTF-32", toRaw = TRUE)[[1]]
    expect_equal(utf32_to_text(s_utf32), s)
})


test_that("utf32_to_utf8 works", {
    s <- "a𐐀𝜺"
    s_utf32 <- utf8_to_utf32(s)
    expect_equal(utf32_to_utf8(s_utf32), as.utfstring(s))

    s_utf32 <- utf8_to_utf32(s, endian = "big")
    expect_equal(utf32_to_utf8(s_utf32), as.utfstring(s))

    s_utf32 <- iconv(s, from = "UTF-8", to = "UTF-32", toRaw = TRUE)[[1]]
    print(s_utf32)
    print(utf32_to_utf8(s_utf32))
    print(as.utfstring(s))
    expect_equal(utf32_to_utf8(s_utf32), as.utfstring(s))
})
