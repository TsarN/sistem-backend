#include <stdio.h>

int main()
{
    printf("%s", "Here goes infinite loop!");
    fflush(stdout);
    while (1);
    return 0;
}
