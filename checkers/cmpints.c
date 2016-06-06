#include <checker.h>
#include <stdio.h>

int checker_main()
{
    checker_init(
"Compare int checker\n\
Checks if exactly one int matches pattern");

    int output = read_int(&f_output);
    expect_eof(&f_output);

    int pattern = read_int(&f_pattern);
    expect_eof(&f_pattern);

    if (output == pattern)
    {
        fprintf(stderr, "ok\n");
        return CR_OK;
    }
    else
    {
        fprintf(stderr, "wrong answer: %d found, %d expected\n", output, pattern);
        return CR_WA;
    }
}
