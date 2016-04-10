#include <checker.h>

int checker_main()
{
    generator_init("EXAMPLE"); /* The source of TRUE randomness */
    if (argc > 1)
        randomize(atoi(argv[1]));
    printf("%s", ask_rand_pattern("([a-z]{8})(\n){8}"));
    return 0;
}
