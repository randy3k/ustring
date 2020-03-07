test_that("cp works", {
    s <- "a𐐀𝜺"
    for (x in list(
            s,
            text_to_utf8(s),
            text_to_utf16(s),
            text_to_utf16(s, endian = "big"),
            text_to_utf32(s),
            text_to_utf32(s, endian = "big"))) {
        expect_equal(npt(x), 3)
        expect_equal(codept(x), c(97L, 66560L, 120634L))
    }
})
