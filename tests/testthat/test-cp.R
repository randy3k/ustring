test_that("code_points works", {
    s <- "a𐐀𝜺"
    for (x in list(
            s,
            text_to_utf8(s),
            text_to_utf16(s),
            text_to_utf16(s, endian = "little"),
            text_to_utf32(s),
            text_to_utf32(s, endian = "little"))) {
        expect_equal(n_code_points(x), 3)
        expect_equal(code_points(x), c(97L, 66560L, 120634L))
    }
})
