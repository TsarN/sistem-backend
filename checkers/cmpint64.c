#include <checker.h>

int checker_main()
{
    checker_init(
"Compare int64 checker\n\
Checks if exactly one int64 matches pattern");

    long output = read_long_long(&f_output);
    expect_eof(&f_output);

    long pattern = read_long_long(&f_pattern);
    expect_eof(&f_pattern);

    if (output == pattern)
        return CR_OK;
    else
        return CR_WA;
}
