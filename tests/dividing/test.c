#include <stdio.h>

int main()
{
    printf("%s", "Dividing by zero test\n");
    int a = 42, b;
    b = a / 22 - 1;
    printf("a / b = %d", a / b);
    return 0;
}
