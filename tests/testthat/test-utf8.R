test_that("utf8 works for string", {
    expect_equal(utf8_len("a𐐀𠍲"), 3)
    expect_equal(utf8_codelen("a𐐀𠍲"), c(1L, 4L, 4L))
    expect_equal(utf8_code("a𐐀𠍲"), c(97L, 66560L, 131954L))
})

test_that("utf8 works for raw", {
    expect_equal(utf8_len(charToRaw("a𐐀𠍲")), 3)
    expect_equal(utf8_codelen(charToRaw("a𐐀𠍲")), c(1L, 4L, 4L))
    expect_equal(utf8_code(charToRaw("a𐐀𠍲")), c(97L, 66560L, 131954L))
})


test_that("utf8_to_utf32 on string works", {
    s <- utf8_to_utf32("a𐐀𠍲")
    expect_equal(length(s), 12)
    expect_equal(s[1:4], as.raw(c(97, 0, 0, 0)))
    expect_equal(s[5:8], as.raw(c(0, 4, 1, 0)))
    expect_equal(s[9:12], as.raw(c(114, 3, 2, 0)))

    s <- utf8_to_utf32("a𐐀𠍲", endian = "big")
    expect_equal(length(s), 12)
    expect_equal(s[1:4], rev(as.raw(c(97, 0, 0, 0))))
    expect_equal(s[5:8], rev(as.raw(c(0, 4, 1, 0))))
    expect_equal(s[9:12], rev(as.raw(c(114, 3, 2, 0))))
})
