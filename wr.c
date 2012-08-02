/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Watch Resources
 *  ---------------
 *  Copyright (C) 2012, Eduardo Silva P. <edsiper@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "wr.h"
#include "proc.h"

static void wr_banner()
{
}

static void wr_help()
{
    printf("Usage: ./wr [-c N] [-l N] [-s N] -p PID http://URL\n\n");
    printf("%sAvailable options%s\n", ANSI_BOLD, ANSI_RESET);
    printf("  -r  \tnumber of requests (default: %i)\n", WR_REQUESTS);
    printf("  -c  \tinitial concurrency (default: %i)\n", WR_CONC_FROM);
    printf("  -l  \tmax concurrency after each step (default: %i)\n", WR_CONC_TO);
    printf("  -s  \tnumber of concurrency steps (default: %i)\n", WR_CONC_STEP);
    printf("  -v  \tshow version number\n");
    printf("  -h, \tprint this help\n\n");
    fflush(stdout);
}

static void wr_version()
{
}

long spawn_benchmark(const char *cmd)
{
    char *s, *p;
    const size_t buf_size = 4096;
    char buf[buf_size];
    FILE *f;

    f = popen(cmd, "r");

    /* Find the results line */
    while (fgets(buf, sizeof(buf) - 1, f)) {
        if (strncmp(buf, "finished ", 9) == 0) {
            s = strstr(buf, " req/s,");
            if (!s) {
                printf("Error: Invalid benchmarking tool output\n");
                exit(EXIT_FAILURE);
            }
            p = (s - 1);
            while (*p != ' ') *p--;
            *p++;

            strncpy(buf, p, (s - p));
            buf[(s - p)] = '\0';
            break;
        }
    }
    fclose(f);

    return atol(buf);
}

void bench_time(time_t t)
{
    const int one_day  = 86400;
    const int one_hour = 3600;
    const int one_min  = 60;

    int days;
    int hours;
    int minutes;
    int seconds;
    long int upmind;
    long int upminh;

    /* days */
    days = t / one_day;
    upmind = t - (days * one_day);

    /* hours */
    hours = upmind / one_hour;
    upminh = upmind - hours * one_hour;

    /* minutes */
    minutes = upminh / one_min;
    seconds = upminh - minutes * one_min;

    printf("Elapsed time: %i day%s, %i hour%s, %i minute%s and %i second%s\n\n",
           days, (days > 1) ? "s" : "", hours, (hours > 1) ? "s" : "", minutes,
           (minutes > 1) ? "s" : "", seconds, (seconds > 1) ? "s" : "");
}

int main(int argc, char **argv)
{
    int opt;
    int pid = -1;
    int keepalive  = 0;
    int conc_from  = WR_CONC_FROM;
    int conc_to    = WR_CONC_TO;
    int conc_step  = WR_CONC_STEP;
    int conc_bench = conc_from;
    int requests   = WR_REQUESTS;
    long req_sec   = 0;
    const size_t buf_size = 4096;
    char buf[buf_size];
    time_t init_time;
    time_t end_time;
    struct wr_proc_task *task_old, *task_new;

    wr_banner();

    while ((opt = getopt(argc, argv, "vhkr:c:s:l:p:")) != -1) {
        switch (opt) {
        case 'v':
            wr_version();
            exit(EXIT_SUCCESS);
        case 'h':
            wr_help();
            exit(EXIT_SUCCESS);
        case 'k':
            keepalive = 1;
            break;
        case 'r':
            requests = atoi(optarg);
            break;
        case 'c':
            conc_from = atoi(optarg);
            break;
        case 'l':
            conc_to = atoi(optarg);
            break;
        case 's':
            conc_step = atoi(optarg);
            break;
        case 'p':
            pid = atoi(optarg);
            break;
        case '?':
            printf("Error: Invalid option or option needs an argument.\n");
            wr_help();
            exit(EXIT_FAILURE);
        }
    }

    if (pid <= 0) {
        printf("Error: Process ID (PID) not specified\n\n");
        wr_help();
        exit(EXIT_FAILURE);
    }

    /* Get the URL */
    if (strncmp(argv[argc - 1], "http", 4) != 0) {
        printf("Error: Invalid URL\n\n");
        wr_help();
        exit(EXIT_FAILURE);
    }

    /* Kernel information: PAGESIZE and CPU_HZ */
    wr_pagesize  = sysconf(_SC_PAGESIZE);
    wr_cpu_hz    = sysconf(_SC_CLK_TCK);

    /* Initial details */
    task_old = wr_proc_stat(pid);
    printf("Process ID  : %i\n", pid);
    printf("Process name: %s\n", task_old->comm);
    wr_proc_free(task_old);

    printf("Concurrency : from %i to %i, step %i\n", conc_from, conc_to, conc_step);

    /* Command */
    memset(buf, '\0', sizeof(buf));
    snprintf(buf, buf_size - 1, BC_BIN,
             requests, conc_from, keepalive > 0 ? "-k": "", argv[argc - 1]);
    printf("Command     : '%s'\n\n", buf);

    /* Table header */
    printf("concurrency  requests/second  user time (ms)  system time (ms)  Mem (bytes)   Mem unit \n");
    printf("-----------  ---------------  --------------  ----------------  -----------   ---------\n");

    init_time = time(NULL);
    conc_bench = conc_from;
    while (conc_bench <= conc_to) {
        task_old = wr_proc_stat(pid);
        req_sec = spawn_benchmark(buf);
        task_new = wr_proc_stat(pid);

        printf("%11ld %16ld %15ld %17ld %12ld %11s\n",
               conc_bench,
               req_sec,                                              /* request per second */
               (task_new->wr_utime_ms - task_old->wr_utime_ms),      /* user time in ms    */
               (task_new->wr_stime_ms - task_old->wr_stime_ms),
               task_new->wr_rss,
               task_new->wr_rss_hr);
        wr_proc_free(task_old);
        wr_proc_free(task_new);

        /* Prepare the command */
        conc_bench += conc_step;
        memset(buf, '\0', sizeof(buf));
        snprintf(buf, buf_size - 1, BC_BIN,
                 requests, conc_bench, " -k ", "http://127.0.0.1:2001/linux.jpg");
    }

    printf("\n--\n");
    end_time = time(NULL);
    bench_time((end_time - init_time));
    fflush(stdout);
    return 0;
}
