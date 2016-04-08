#include <stdio.h>

int main()
{
    FILE *f = fopen("file.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return 0;
    }

    /* print some text */
    const char *text = "Write this to the file";
    fprintf(f, "Some text: %s\n", text);
    fclose(f);
    return 0;
}
