#include <checker.h>
#include <stdio.h>

int checker_main()
{
    checker_init(
"Compare int64 checker\n\
Checks if exactly one int64 matches pattern");

    long long output = read_long_long(&f_output);
    expect_eof(&f_output);

    long long pattern = read_long_long(&f_pattern);
    expect_eof(&f_pattern);

    if (output == pattern)
    {
        fprintf(stderr, "ok\n");
        return CR_OK;
    }
    else
    {
        fprintf(stderr, "wrong answer: %lld found, %lld expected\n", output, pattern);
        return CR_WA;
    }
}
