test_that("utf8 on string works", {
    expect_equal(ncodept("a𐐀𝜺"), 3)
    expect_equal(codept("a𐐀𝜺"), c(97L, 66560L, 120634L))
})

test_that("utf8 on raw works", {
    expect_equal(ncodept(as.utfstring("a𐐀𝜺")), 3)
    expect_equal(ncodept(charToRaw("a𐐀𝜺")), 3)
    expect_equal(codept(charToRaw("a𐐀𝜺")), c(97L, 66560L, 120634L))
    expect_equal(codept(charToRaw("fa\xE7ile"))[3], NA_integer_)
})



test_that("utf8_to_utf16 on string works", {
    s <- "a𐐀𝜺"
    s_utf16 <- utf8_to_utf16(s)
    expect_equal(length(s_utf16), 10)
    expect_equal(s_utf16[1:2], as.raw(c(97, 0)))
    expect_equal(s_utf16[3:6], as.raw(c(1, 216, 0, 220)))
    expect_equal(s_utf16[7:10], as.raw(c(53, 216, 58, 223)))

    s <- "a𐐀𝜺"
    s_utf16 <- utf8_to_utf16(s, endian = "big")
    expect_equal(length(s_utf16), 10)
    expect_equal(s_utf16[1:2], as.raw(c(0, 97)))
    expect_equal(s_utf16[3:6], as.raw(c(216, 1, 220, 0)))
    expect_equal(s_utf16[7:10], as.raw(c(216, 53, 223, 58)))
})


test_that("utf8_to_utf16 on raw works", {
    s <- as.utfstring("a𐐀𝜺")
    s_utf16 <- utf8_to_utf16(s)
    expect_equal(length(s_utf16), 10)
    expect_equal(s_utf16[1:2], as.raw(c(97, 0)))
    expect_equal(s_utf16[3:6], as.raw(c(1, 216, 0, 220)))
    expect_equal(s_utf16[7:10], as.raw(c(53, 216, 58, 223)))

    s <- as.utfstring("a𐐀𝜺")
    s_utf16 <- utf8_to_utf16(s, endian = "big")
    expect_equal(length(s_utf16), 10)
    expect_equal(s_utf16[1:2], as.raw(c(0, 97)))
    expect_equal(s_utf16[3:6], as.raw(c(216, 1, 220, 0)))
    expect_equal(s_utf16[7:10], as.raw(c(216, 53, 223, 58)))
})


test_that("utf8_to_utf32 on string works", {
    s <- "a𐐀𝜺"
    s_utf32 <- utf8_to_utf32(s)
    expect_equal(length(s_utf32), 12)
    expect_equal(s_utf32[1:4], as.raw(c(97, 0, 0, 0)))
    expect_equal(s_utf32[5:8], as.raw(c(0, 4, 1, 0)))
    expect_equal(s_utf32[9:12], as.raw(c(58, 215, 1, 0)))

    s <- "a𐐀𝜺"
    s_utf32 <- utf8_to_utf32(s, endian = "big")
    expect_equal(length(s_utf32), 12)
    expect_equal(s_utf32[1:4], rev(as.raw(c(97, 0, 0, 0))))
    expect_equal(s_utf32[5:8], rev(as.raw(c(0, 4, 1, 0))))
    expect_equal(s_utf32[9:12], rev(as.raw(c(58, 215, 1, 0))))
})


test_that("utf8_to_utf32 on raw works", {
    s <- as.utfstring("a𐐀𝜺")
    s_utf32 <- utf8_to_utf32(s)
    expect_equal(length(s_utf32), 12)
    expect_equal(s_utf32[1:4], as.raw(c(97, 0, 0, 0)))
    expect_equal(s_utf32[5:8], as.raw(c(0, 4, 1, 0)))
    expect_equal(s_utf32[9:12], as.raw(c(58, 215, 1, 0)))

    s <- as.utfstring("a𐐀𝜺")
    s_utf32 <- utf8_to_utf32(s, endian = "big")
    expect_equal(length(s_utf32), 12)
    expect_equal(s_utf32[1:4], rev(as.raw(c(97, 0, 0, 0))))
    expect_equal(s_utf32[5:8], rev(as.raw(c(0, 4, 1, 0))))
    expect_equal(s_utf32[9:12], rev(as.raw(c(58, 215, 1, 0))))
})
