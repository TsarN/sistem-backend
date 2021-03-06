#include <checker.h>
#include <stdio.h>

int checker_main()
{
    checker_init(
"A + B Checker\n\
Checks if two numbers given on input are sum of number given on output");

    int a = read_int(&f_input);
    int b = read_int(&f_input);
    expect_eof(&f_input);

    int res = read_int(&f_output);
    expect_eof(&f_output);

    if (a + b == res)
    {
        fprintf(stderr, "ok\n");
        return CR_OK;
    }
    else
    {
        fprintf(stderr, "wrong answer: %d found, %d expected\n", res, a + b);
        return CR_WA;
    }
}
