#ifdef __linux__
#include <unistd.h>
#endif
#include <stdio.h>

int main()
{
#ifdef __linux__
    printf("%d", fork());
#else
printf("-1");
#endif
}
