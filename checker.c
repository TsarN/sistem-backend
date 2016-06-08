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


#include <checker.h>

bool fatal = false;

bool is_eof(char c)
{
    return (c == '\0');
}

bool is_eof_stream(str_stream *stream)
{
    return is_eof(stream->str[stream->pos]);
}

bool is_eoln(char c)
{
    return (c == LF) || (c == CR);
}

bool is_eoln_stream(str_stream *stream)
{
    return is_eoln(stream->str[stream->pos]);
}

bool is_blank(char c)
{
    return (c == LF) || (c == CR) || (c == SPACE) || (c == TAB);
}

bool is_blank_stream(str_stream *stream)
{
    return is_blank(stream->str[stream->pos]);
}

void str_stream_error(str_stream *stream, int err)
{
    stream->last_err = err;
    if (fatal || stream->fatal)
    {
        fprintf(stderr, "%s\n", ERR_NAMES[err]);
        if (stream->fatal)
        {
            fprintf(stderr, "%s\n", "Invalid test");
            exit(CR_IT);
        }
        else
        {
            fprintf(stderr, "%s\n", "Presentation error");
            exit(CR_PE);
        }
    }
}

str_stream new_str_stream(char *str)
{
    str_stream res;
    res.str = str;
    res.pos = 0;
    res.last_err = 0;
    res.fatal = false;
    return res;
}

str_stream new_str_stream_file(char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        fprintf(stderr, "%s\n", "Failed to create str_stream from file: couldn't open file");
        exit(CR_IE);
    }

    fseek(f, 0, SEEK_END);
    size_t f_length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *str = malloc(f_length);
    if (!str)
    {
        fprintf(stderr, "%s\n", "Failed to create str_stream from file: couldn't allocate memory");
        exit(CR_IE);
    }

    fread(str, 1, f_length, f);
    fclose(f);

    return new_str_stream(str);
}

void seek_visible(str_stream *stream)
{
    for (;is_blank_stream(stream); ++stream->pos);
}

void seek_eoln(str_stream *stream)
{
    for (;is_blank_stream(stream) && !is_eoln_stream(stream); ++stream->pos);
}

void seek_newline(str_stream *stream)
{
    for (;is_eoln_stream(stream); ++stream->pos);
}

char *read_line(str_stream *stream)
{
    seek_visible(stream);
    if (!stream->str[stream->pos])
    {
        str_stream_error(stream, ERR_UNEXPECTED_EOF);
        return NULL;
    }
    char *begin = stream->str + stream->pos;
    size_t len = 0;
    for (;!is_eoln(stream->str[stream->pos]); ++stream->pos) ++len;
    char *str = malloc(len + 1);
    if (!str)
    {
        fprintf(stderr, "%s\n", "Failed to read from str_stream: couldn't allocate memory");
        exit(CR_IE);
    }
    memcpy(str, begin, len);
    str[len] = (char)0;
    return str;
}

char *read_string(str_stream *stream)
{
    seek_visible(stream);
    if (!stream->str[stream->pos])
    {
        str_stream_error(stream, ERR_UNEXPECTED_EOF);
        return NULL;
    }
    char *begin = stream->str + stream->pos;
    size_t len = 0;
    for (;!is_blank(stream->str[stream->pos]); ++stream->pos) ++len;
    char *str = malloc(len + 1);
    if (!str)
    {
        fprintf(stderr, "%s\n", "Failed to read from str_stream: couldn't allocate memory");
        exit(CR_IE);
    }
    memcpy(str, begin, len);
    str[len] = (char)0;
    return str;
}
char *read_big_integer(str_stream *stream)
{
    char *str_int = read_string(stream);
    bool valid = true;
    for (char *ptr = str_int; *ptr; ++ptr)
    {
        if (((*ptr < '0') || (*ptr > '9')) && (*ptr != '-') && (*ptr != '+'))
        {
            valid = false;
            break;
        }
    }
    if (!valid)
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_int);
        return NULL;
    }

    return str_int;
}

long long read_long_long(str_stream *stream)
{
    char *str_int = read_big_integer(stream);
    if (!str_int)
    {
        return 0;
    }
    long long res;
    if (!sscanf(str_int, LLD_FORMAT, &res))
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_int);
        return 0;
    }
    free(str_int);
    return res;
}

unsigned long long read_unsigned_long_long(str_stream *stream)
{
    char *str_int = read_big_integer(stream);
    if (!str_int)
    {
        return 0;
    }
    long long res;
    if (!sscanf(str_int, LLU_FORMAT, &res))
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_int);
        return 0;
    }
    free(str_int);
    return res;
}

long long read_integer_in_bounds(str_stream *stream, long long min_bound, long long max_bound)
{
    long long integer = read_long_long(stream);
    if (stream->last_err)
    {
        return 0;
    }
    if ((integer >= min_bound) && (integer <= max_bound))
    {
        return integer;
    }
    else
    {
        str_stream_error(stream, ERR_OUT_OF_BOUNDS);
        return 0;
    }
}

unsigned long long read_unsigned_integer_in_bounds(str_stream *stream, unsigned long long min_bound, unsigned long long max_bound)
{
    unsigned long long integer = read_unsigned_long_long(stream);
    if (stream->last_err)
    {
        return 0;
    }

    if ((integer >= min_bound) && (integer <= max_bound))
    {
        return integer;
    }
    else
    {
        str_stream_error(stream, ERR_OUT_OF_BOUNDS);
        return 0;
    }
}

long read_long(str_stream *stream)
{
    return (long)read_integer_in_bounds(stream, LONG_MIN, LONG_MAX);
}

unsigned long read_unsigned_long(str_stream *stream)
{
    return (unsigned long)read_unsigned_integer_in_bounds(stream, 0, ULONG_MAX);
}

int read_int(str_stream *stream)
{
    return (int)read_integer_in_bounds(stream, INT_MIN, INT_MAX);
}

unsigned int read_unsigned_int(str_stream *stream)
{
    return (unsigned int)read_unsigned_integer_in_bounds(stream, 0, UINT_MAX);
}

long double read_long_double(str_stream *stream)
{
    char *str_float = read_string(stream);
    if (!str_float)
    {
        return 0;
    }
    LDOUBLE_CAST res;
    if (!sscanf(str_float, LF_FORMAT, &res))
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_float);
        return 0;
    }
    free(str_float);
    return (long double)res;
}

double read_double(str_stream *stream)
{
    return (double)read_long_double(stream);
}

float read_float(str_stream *stream)
{
    return (float)read_long_double(stream);
}

char read_character(str_stream *stream)
{
    for (; is_eoln(stream->str[stream->pos]); ++stream->pos);
    if (is_eof_stream(stream))
    {
        str_stream_error(stream, ERR_UNEXPECTED_EOF);
        return '\0';
    }
    return stream->str[stream->pos++];
}

void expect_eof(str_stream *stream)
{
    seek_visible(stream);
    if (!is_eof_stream(stream))
    {
        str_stream_error(stream, ERR_EOF_EXPECTED);
    }
}

void expect_eoln(str_stream *stream)
{
    seek_eoln(stream);
    if (!is_eoln_stream(stream))
    {
        str_stream_error(stream, ERR_EOLN_EXPECTED);
    }
    seek_newline(stream);
}

void checker_init_impl(int argc, char **argv, char *description)
{
    if (argc > 1)
    {
        if (!strcmp(argv[1], "--show-description"))
        {
            printf("%s\n", description);
            exit(0);
        }
    }
    if (argc <= 2)
    {
        fprintf(stderr, "%s\n", "At least two arguments required to run this checker");
        fprintf(stderr, "%s\n", "Use --show-description to get checker description");
        exit(CR_IE);
    }

    char *file_input_name = argv[1];
    char *file_output_name = argv[2];
    char *file_pattern_name = (argc > 3)?argv[3]:NULL;

    f_input = new_str_stream_file(file_input_name);
    f_input.fatal = true;

    f_output = new_str_stream_file(file_output_name);

    if (file_pattern_name)
    {
        f_pattern = new_str_stream_file(file_pattern_name);
    }
    else
    {
        f_pattern.str = malloc(1);
        f_pattern.str[0] = '\0';
        f_pattern.pos = 0;
        f_pattern.last_err = 0;
    }
    f_pattern.fatal = true; 

    fatal = true; 
}

void generator_init_impl(int argc, char **argv, char *checker_string)
{
    /* Generating random seed from secret string */
    unsigned long long new_seed = rnd_seed;
    for (char *c = checker_string; *c; ++c)
        new_seed = new_seed * 57931 + *c;
    rnd_seed = new_seed ? new_seed : rnd_seed;
}

void randomize(int x)
{
    unsigned long long new_seed = rnd_seed * 57931 * x;
    rnd_seed = new_seed ? new_seed : rnd_seed;
}

unsigned long long ask_rand()
{
    rnd_seed ^= rnd_seed >> 12;
    rnd_seed ^= rnd_seed << 25;
    rnd_seed ^= rnd_seed >> 27;
    return rnd_seed * (unsigned long long)2685821657736338717ULL;
}

unsigned long long ask_rand_in_bounds(unsigned long long lower, unsigned long long higher)
{
    return ask_rand() % (higher - lower + 1) + lower;
}

double ask_rand_double()
{
    return (double)ask_rand() / (double)ULLONG_MAX;
}

int ask_rand_pattern_total_escaped = 0;
int *ask_rand_pattern_escaped = NULL;
char *ask_rand_call_pattern = NULL;

bool ask_rand_pattern_is_escaped(char *c)
{
    int pos = (c - ask_rand_call_pattern);
    for (int i = 0; i < ask_rand_pattern_total_escaped; ++i)
    {
        if (ask_rand_pattern_escaped[i] == pos)
        {
            return true;
        }
    }
    return false;
}

bool ask_rand_pattern_special_equal(char *c, char eq)
{
    return (*c == eq) && (!ask_rand_pattern_is_escaped(c));
}

char *ask_rand_pattern_impl(char *begin, char *end)
{
    char *result = malloc(1);
    int result_len = 0;
    result[0] = '\0';
 
    /* Looking for OR statements */

    int or_num = 0;

    for (char *c = begin; c != end; ++c)
    {
        if (ask_rand_pattern_special_equal(c, '|'))
            or_num++;
    }

    if (or_num > 0)
    {
        or_num++; 
        char **or_exprs = malloc(sizeof(char *) * or_num);
        for (int i = 0; i < or_num; ++i)
        {
            or_exprs[i] = NULL;
        }

        int cur_expr = 0;
        char *cur_expr_begin = begin;

        for (char *c = begin; c <= end; ++c)
        {
            if ((c == end) || (ask_rand_pattern_special_equal(c, '|')))
            {
                or_exprs[cur_expr] = malloc(c - cur_expr_begin + 1);
                memcpy(or_exprs[cur_expr], cur_expr_begin, c - cur_expr_begin);
                or_exprs[cur_expr][c - cur_expr_begin] = '\0';
                cur_expr ++;
                cur_expr_begin = c + 1;
                continue;
            }
        }

        int expr = (int)ask_rand_in_bounds(0, or_num - 1);

        char *ans = ask_rand_pattern_impl(or_exprs[expr], or_exprs[expr] + strlen(or_exprs[expr]));
        for (int i = 0; i < or_num; ++i)
        {
            free(or_exprs[i]);
        }
        free(or_exprs);
        free(result);

        return ans;
    }

    /* Looking for character generators */

    int n_generators = 0;
    for (char *c = begin; c != end; ++c)
    {
        if (ask_rand_pattern_special_equal(c, '['))
            n_generators++;
    }

    bool subpattern_generators = false;

    if (n_generators > 1)
    {
        subpattern_generators = true;
    }

    if ((n_generators == 1) && (*(end - 1) == ']'))
    {
        int total = 0;
        for (char *c = begin + 1; c != end - 1; ++c)
        {
            if (ask_rand_pattern_special_equal(c + 1, '-') || ask_rand_pattern_special_equal(c - 1, '-'))
                continue;
            if (ask_rand_pattern_special_equal(c, '-'))
            {
                total += max(0, *(c + 1) - *(c - 1));
            }
            total++;
        }
        char *symlist = malloc(total);
        int added = 0;
        for (char *c = begin + 1; c != end - 1; ++c)
        {
            if (ask_rand_pattern_special_equal(c + 1, '-') || ask_rand_pattern_special_equal(c - 1, '-'))
                continue;
            if (ask_rand_pattern_special_equal(c, '-'))
            {
                for (char cc = *(c - 1); cc <= *(c + 1); ++cc)
                {
                    symlist[added] = cc;
                    added++;
                }
            }
            symlist[added] = *c;
            added++;
        }
        int idx = (int)ask_rand_in_bounds(0, total - 1);
        char ans = symlist[idx];
        free(symlist);
        
        result = realloc(result, 2);
        result[0] = ans;
        result[1] = '\0';
        return result;
    }

    char *quantifier_begin = NULL;

    int bal = 0;

    for (char *c = begin; c != end; ++c)
    {
        if (ask_rand_pattern_special_equal(c, '('))
            bal++;
        if (ask_rand_pattern_special_equal(c, ')'))
            bal--;
        if (ask_rand_pattern_special_equal(c, '{') && !bal)
            quantifier_begin = c;
    }

    /* Looking for subpatterns */
    bool has_subpatterns = false;
    char *subpattern_begin = NULL;
    bal = 0;

    if (!quantifier_begin)
    {
        for (char *c = begin; c != end; ++c)
        {
            if (ask_rand_pattern_special_equal(c, '(') ||
            (subpattern_generators && ask_rand_pattern_special_equal(c, '[')))
            {
                has_subpatterns = true;
                if (!subpattern_begin)
                subpattern_begin = (*c == '[') ? c : (c + 1);
                bal++;
            }
            if (ask_rand_pattern_special_equal(c, ')') ||
            (subpattern_generators && ask_rand_pattern_special_equal(c, ']')))
            {
                bal--;
                if (!bal)
                {
                    char *subpattern_end = (*c == ']') ? (c + 1) : c;
                    char *res = ask_rand_pattern_impl(subpattern_begin, subpattern_end);
                    subpattern_begin = NULL;
                    int res_len = strlen(res);
                    result = realloc(result, result_len + res_len + 1);
                    memcpy(result + result_len, res, res_len + 1);
                    result_len += res_len;
                    free(res);
                }
            }
        }
        if (has_subpatterns)
            return result;
    }
    else
    {
        int amount = atoi(quantifier_begin + 1);
        int ramount = -1;
        for (char *c = quantifier_begin + 1; c != end; ++c)
        {
            if (*c == ',')
            {
                ramount = atoi(c + 1);
                break;
            }
        }
        if (ramount != -1)
            amount = (int)ask_rand_in_bounds(amount, ramount);
        size_t result_len = 0;
        for (int i = 0; i < amount; ++i)
        { 
            char *cur = ask_rand_pattern_impl(begin, quantifier_begin);
            size_t cur_len = strlen(cur);
            result = realloc(result, result_len + cur_len + 1);
            strcpy(result + result_len, cur);
            free(cur);
            result_len += cur_len;
        }
        return result;
    }

    free(result);
    result = malloc(end - begin + 1);
    memcpy(result, begin, end - begin);
    result[end - begin] = '\0';
    return result;
}

char *ask_rand_pattern(char *pattern)
{
    ask_rand_pattern_total_escaped = 0;
    for (char *c = pattern; *c; ++c)
    {
        if (*c == '\\')
        {
            ++c;
            ask_rand_pattern_total_escaped ++;
        }
    }

    ask_rand_pattern_escaped = malloc(ask_rand_pattern_total_escaped * sizeof(int));
    int npos = 0;
    int new_pattern_len = strlen(pattern) - ask_rand_pattern_total_escaped;
    char *new_pattern = malloc(new_pattern_len + 1);
    int pos = 0;

    for (char *c = pattern; *c; ++c)
    {
        if (*c == '\\')
        {
            ask_rand_pattern_escaped[npos] = pos;
            npos++;
            continue;
        }
        else
        {
            new_pattern[pos] = *c;
            pos++;
        }
    }
    new_pattern[new_pattern_len] = '\0';
    ask_rand_call_pattern = new_pattern;

    char *ret = ask_rand_pattern_impl(new_pattern, new_pattern + new_pattern_len);
    free(new_pattern);
    return ret;
}


long long ask_weighted_rand_in_bounds(long long lower, long long upper, int weight)
{
    if (weight >= 0)
    {
        long long ans = LLONG_MIN;
        for (int i = 0; i <= weight; ++i)
        {
            long long r = (signed long long)ask_rand_in_bounds(lower + LLONG_MAX, upper + LLONG_MAX) - LLONG_MAX;
            ans = max(ans, r);
        }
        return ans;
    }
    else
    {
        long long ans = LLONG_MAX;
        for (int i = 0; i >= weight; --i)
        {
            long long r = (signed long long)ask_rand_in_bounds(lower + LLONG_MAX, upper + LLONG_MAX) - LLONG_MAX;
            ans = min(ans, r);
        }
        return ans;
    }
}
