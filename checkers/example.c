/* This is an example checker with explanations 
 * It compares int in input file and int in output file 
 * To create executable, compile & link it with libchecker
 */

#include <checker.h> /* Required include */
#include <stdio.h> /* Logging */

int checker_main() /* Required definition */
{
    /* checker_init(description) - Required call */
    checker_init(
"Example Checker\n\
Checks if number given on input is a number given on output");

    int x = read_int(&f_input); /* Read one 32-bit integer from input file */
    expect_eof(&f_input); /* Input file must not contain any other data.
                           * If it does - raise Invalid Test error
                           */

    int y = read_int(&f_output); /* Read one 32-bit integer from user's program output */
    expect_eof(&f_output); /* Output file must not contain any other data.
                            * If it does - raise Presentation Error
                            */

    /* Optionally, test can contain pattern file
     * It acts similiar to input file
     */

    if (x == y) /* Compare two numbers */
    {
        fprintf(stderr, "ok\n"); /* Add some commentary */
        return CR_OK; /* This test passed. Return OK */
    }
    else
    {
        fprintf(stderr, "wrong answer: %d found, %d expected\n", y, x); /* 'y' is user's output, 'x' is expected */
        return CR_WA; /* This test failed. Return Wrong Answer */
    }
}
