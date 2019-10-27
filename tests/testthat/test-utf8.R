test_that("utf8 works for string", {
    expect_equal(utf8_length("a𐐀𠍲"), 3)
    expect_equal(utf8_codelen("a𐐀𠍲"), c(1L, 4L, 4L))
    expect_equal(utf8_code("a𐐀𠍲"), c(97L, 66560L, 131954L))
})

test_that("utf8 works for raw", {
    expect_equal(utf8_length(charToRaw("a𐐀𠍲")), 3)
    expect_equal(utf8_codelen(charToRaw("a𐐀𠍲")), c(1L, 4L, 4L))
    expect_equal(utf8_code(charToRaw("a𐐀𠍲")), c(97L, 66560L, 131954L))
    # latin1 string as raw
    expect_equal(utf8_code(charToRaw("fa\xE7ile"))[3], NA_integer_)
})


test_that("utf8_to_utf32 on string works", {
    s <- "a𐐀𠍲"
    s_utf32 <- utf8_to_utf32(s)
    expect_equal(length(s_utf32), 12)
    expect_equal(s_utf32[1:4], as.raw(c(97, 0, 0, 0)))
    expect_equal(s_utf32[5:8], as.raw(c(0, 4, 1, 0)))
    expect_equal(s_utf32[9:12], as.raw(c(114, 3, 2, 0)))

    s <- "a𐐀𠍲"
    s_utf32 <- utf8_to_utf32(s, endian = "big")
    expect_equal(length(s_utf32), 12)
    expect_equal(s_utf32[1:4], rev(as.raw(c(97, 0, 0, 0))))
    expect_equal(s_utf32[5:8], rev(as.raw(c(0, 4, 1, 0))))
    expect_equal(s_utf32[9:12], rev(as.raw(c(114, 3, 2, 0))))
})


test_that("utf32_to_utf8 on string works", {
    s <- "a𐐀𠍲"
    s_utf32 <- utf8_to_utf32(s)
    expect_equal(utf32_to_utf8(s_utf32), s)

    s_utf32 <- utf8_to_utf32(s, endian = "big")
    expect_equal(utf32_to_utf8(s_utf32), s)

    s_utf32 <- iconv(s, from = "UTF-8", to = "UTF-32", toRaw = TRUE)[[1]]
    expect_equal(utf32_to_utf8(s_utf32), s)
})



test_that("utf8_to_utf16 on string works", {
    s <- "a𐐀𠍲"
    s_utf16 <- utf8_to_utf16(s)
    expect_equal(length(s_utf16), 10)
    expect_equal(s_utf16[1:2], as.raw(c(97, 0)))
    expect_equal(s_utf16[3:6], as.raw(c(1, 216, 0, 220)))
    expect_equal(s_utf16[7:10], as.raw(c(64, 216, 114, 223)))

    s <- "a𐐀𠍲"
    s_utf16 <- utf8_to_utf16(s, endian = "big")
    expect_equal(length(s_utf16), 10)
    expect_equal(s_utf16[1:2], as.raw(c(0, 97)))
    expect_equal(s_utf16[3:6], as.raw(c(216, 1, 220, 0)))
    expect_equal(s_utf16[7:10], as.raw(c(216, 64, 223, 114)))
})


test_that("utf16_to_utf8 on string works", {
    s <- "a𐐀𠍲"
    s_utf16 <- utf8_to_utf16(s)
    expect_equal(utf16_to_utf8(s_utf16), s)

    s_utf16 <- utf8_to_utf16(s, endian = "big")
    expect_equal(utf16_to_utf8(s_utf16), s)

    s_utf16 <- iconv(s, from = "UTF-8", to = "UTF-16", toRaw = TRUE)[[1]]
    expect_equal(utf16_to_utf8(s_utf16), s)
})
