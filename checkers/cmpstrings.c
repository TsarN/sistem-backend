#include <checker.h>
#include <stdio.h>

int checker_main()
{
    checker_init(
"Compare string checker\n\
Checks if exactly one string matches pattern");

    char *output = read_string(&f_output);
    expect_eof(&f_output);

    char *pattern = read_string(&f_pattern);
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
