#include <checker.h>

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
        return CR_OK;
    else
        return CR_WA;
}
