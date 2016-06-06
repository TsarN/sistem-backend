#include <checker.h>
#include <stdio.h>

int checker_main()
{
    checker_init(
"Compare long checker\n\
Checks if exactly one long matches pattern");

    long output = read_long(&f_output);
    expect_eof(&f_output);

    long pattern = read_long(&f_pattern);
    expect_eof(&f_pattern);

    if (output == pattern)
    {
        fprintf(stderr, "ok\n");
        return CR_OK;
    }
    else
    {
        fprintf(stderr, "wrong answer: %ld found, %ld expected\n", output, pattern);
        return CR_WA;
    }
}
