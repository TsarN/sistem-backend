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

void set_process_flag(int flag)
{
    ptrace(flag, 0, 0, 0);
}

void set_memory_limit(int kb)
{
    struct rlimit rmemlimit; 
    rmemlimit.rlim_cur = kb  *1024;
    rmemlimit.rlim_max = kb  *1024;
    setrlimit(RLIMIT_AS, &rmemlimit);
    set_process_flag(PF_ML);
}

void set_cpu_limit(int ms)
{
    struct rlimit rtimelimit; 
    rtimelimit.rlim_cur = max(ms / 1000, 1);
    rtimelimit.rlim_max = max(ms / 1000, 1);
    setrlimit(RLIMIT_CPU, &rtimelimit);
    set_process_flag(PF_TL);
}

void set_security(void)
{
    set_process_flag(PF_SV);
}


exec_result run_limited_process(char *exe, char *input, int time_limit, int memory_limit)
{
    exec_result result;
    result.flags = ER_OK;
    result.exitcode = 0;
    result.pstdout = malloc(1); /* sizeof (char) == 1 */
    result.pstdout[0] = '\0';

    /* Forking */
    int output_pipe[2];
    int input_pipe[2];
    pipe(output_pipe);
    pipe(input_pipe);
    int pid = fork();
    if (pid == 0)
    {
        /* Close reading end of output */
        close(output_pipe[0]); 
        close(input_pipe[1]);

        /* Send stdout to controller */
        dup2(output_pipe[1], 1); 

        /* Receive stdin from controller */
        dup2(input_pipe[0], 0); 

        /* Enable limitations */
        set_cpu_limit(time_limit);
        set_memory_limit(memory_limit);
        set_security();

        /* Execute program */
        char **args = malloc(2 * sizeof(char *));
        args[0] = exe;
        args[1] = NULL;

        char **env = malloc(2 * sizeof(char *));
        env[0] = "LD_PRELOAD=libc.so.6 libm.so.6"; 
        env[1] = NULL;
        execvpe(exe, args, env);

        /* Something is seriously wrong */
        fprintf(stderr, "execvpe() failed\n");
        result.flags = ER_IE;
        return result;
    }
    else if (pid < 0)
    {
        /* Failed to fork */
        close(input_pipe[0]);
        close(input_pipe[1]);
        close(output_pipe[0]);
        close(output_pipe[1]);
        fprintf(stderr, "fork() failed\n");
        result.flags = ER_IE;
        return result;
    }
    else
    {
        /* Send stdin to program */
        close(input_pipe[0]);
        close(output_pipe[1]);
        write(input_pipe[1], input, sizeof(input));
        close(input_pipe[1]); 

        /* User time limit */
        int pstatus;
        int timeoutpid = fork();
        if (timeoutpid < 0)
        {
            kill(SIGKILL, pid);
            fprintf(stderr, "timeout fork() failed\n");
            result.flags = ER_IE;
            return result;
        }
        if (timeoutpid == 0)
        {
            usleep(time_limit  *1000  *4);
            exit(0);
        }
        int exitedpid = wait(&pstatus);
        {
            if (exitedpid == timeoutpid)
            {
                result.flags |= ER_TL;
                result.flags |= ER_RE;
                kill(SIGKILL, pid);
                return result;
            }
            else
            {
                kill(SIGKILL, timeoutpid);
            }
        }

        /* Receiving stdout */
        char buf[MAX_BUF];
        int z;
        int wsize = 1;
        while ((z = read(output_pipe[0], buf, MAX_BUF)) != 0)
        {
            wsize += z;
            char *tmp  = realloc(result.pstdout, wsize);
            if (tmp)
            {
                result.pstdout = tmp;
            }
            else
            {
                /* realloc() failed */
                fprintf(stderr, "realloc() failed\n");
                result.flags = ER_IE;
                return result;
            }
            strncat(result.pstdout, buf, z);
        } 
        if (pstatus & EC_ML) result.flags |= ER_ML;
        if (pstatus & EC_SV) result.flags |= ER_SV;
        if (pstatus & EC_TL) result.flags |= ER_TL;
        pstatus &= 0xffff;
        if (pstatus)
        {
            result.flags |= ER_RE;
            result.exitcode = pstatus;
        }
        return result;
    }
    return result;
}
