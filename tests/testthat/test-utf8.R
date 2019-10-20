test_that("utf8_len works", {
    expect_equal(utf8_codelen("a𐐀𠍲"), c(1L, 4L, 4L))
})
