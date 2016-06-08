#include <checker.h>
#include <stdio.h>

int checker_main()
{
    checker_init(
"Compare line checker\n\
Checks if exactly one line matches pattern");

    char *output = read_line(&f_output);
    expect_eof(&f_output);

    char *pattern = read_line(&f_pattern);
    expect_eof(&f_pattern);

    if (!strcmp(output, pattern))
    {
        fprintf(stderr, "ok\n");
        return CR_OK;
    }
    else
    {
        fprintf(stderr, "wrong answer: '%s' found, '%s' expected\n", output, pattern);
        return CR_WA;
    }
}
