/*
 * Copyright (c) 2016 Tsarev Nikita
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <sbox.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc <= 4)
    {
        /* Print usage information */
        fprintf(stderr, "%s%s%s", "Usage: ", argv[0], " <program> <time limit, ms> <memory limit, KB> <input file> [output file, default=stdout]\n");
        return ER_IE;
    }

    char *program = argv[1];
    int time_limit = atoi(argv[2]);
    int memory_limit = atoi(argv[3]);
    char *file_in_name = argv[4];
    char *file_out_name = NULL;
    if (argc >= 5)
        file_out_name = argv[5];

    FILE *file_in = fopen(file_in_name, "rb");
    if (!file_in)
    {
        perror("Failed to open input file");
        return ER_IE;
    }

    fseek(file_in, 0, SEEK_END);
    size_t file_in_length = ftell(file_in);
    fseek(file_in, 0, SEEK_SET);

    char *input = malloc(file_in_length);
    if (!input)
    {
        perror("Failed to allocate memory");
        return ER_IE;
    }

    fread(input, 1, file_in_length, file_in);
    fclose(file_in);

    /* Run process */
    exec_result result = run_limited_process(program, input, time_limit, memory_limit);

    free(input);

    if (file_out_name)
    {
        FILE *file_out = fopen(file_out_name, "wb");
        if (!file_out)
        {
            perror("Failed to open output file");
            return ER_IE;
        }

        fprintf(file_out, "%s", result.pstdout);
        fclose(file_out);
    }
    else
    {
        printf("%s", result.pstdout);
    }

    return result.flags;
}
