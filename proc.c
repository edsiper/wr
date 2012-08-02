/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Watch Resources
 *  ------------------
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
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "wr.h"
#include "proc.h"

static char *human_readable_size(long size)
{
    long u = 1024, i, len = 128;
    char *buf = malloc(len);
    static const char *__units[] = { "b", "K", "M", "G",
        "T", "P", "E", "Z", "Y", NULL
    };

    for (i = 0; __units[i] != NULL; i++) {
        if ((size / u) == 0) {
            break;
        }
        u *= 1024;
    }
    if (!i) {
        snprintf(buf, len, "%ld %s", size, __units[0]);
    }
    else {
        float fsize = (float) ((double) size / (u / 1024));
        snprintf(buf, len, "%.2f%s", fsize, __units[i]);
    }

    return buf;
}

/* Read file content to a memory buffer,
 * Use this function just for really SMALL files
 */
static char *file_to_buffer(const char *path)
{
    FILE *fp;
    char *buffer;
    long bytes;

    if (access(path, R_OK) != 0) {
        printf("Error: could not open '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    if (!(fp = fopen(path, "r"))) {
        return NULL;
    }

    buffer = malloc(PROC_STAT_BUF_SIZE);
    if (!buffer) {
        fclose(fp);
        printf("Error: could not allocate memory to open '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    bytes = fread(buffer, PROC_STAT_BUF_SIZE, 1, fp);
    if (bytes < 0) {
        free(buffer);
        fclose(fp);
        printf("Error: could not read '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    fclose(fp);
    return (char *) buffer;
}


struct wr_proc_task *wr_proc_stat(pid_t pid)
{
    int ret;
    char *p, *q;
    char *buf;
    char pid_path[PROC_PID_SIZE];
    struct wr_proc_task *t = malloc(sizeof(struct wr_proc_task));

    memset(t, '\0', sizeof(struct wr_proc_task));

    /* Compose path for /proc/PID/stat */
    ret = snprintf(pid_path, PROC_PID_SIZE, "/proc/%i/stat", pid);
    if (ret < 0) {
        printf("Error: could not compose PID path\n");
        exit(EXIT_FAILURE);
    }

    buf = file_to_buffer(pid_path);
    sscanf(buf, "%d", &t->pid);

    /*
     * workaround for process with spaces in the name, so we dont screw up
     * sscanf(3).
     */
    p = buf;
    while (*p != '(') p++; p++;
    q = p;
    while (*q != ')') q++;
    strncpy(t->comm, p, q - p);
    q += 2;

    /* Read pending values */
    sscanf(q, PROC_STAT_FORMAT,
           &t->state,
           &t->ppid,
           &t->pgrp,
           &t->session,
           &t->tty_nr,
           &t->tpgid,
           &t->flags,
           &t->minflt,
           &t->cminflt,
           &t->majflt,
           &t->cmajflt,
           &t->utime,
           &t->stime,
           &t->cutime,
           &t->cstime,
           &t->priority,
           &t->nice,
           &t->num_threads,
           &t->itrealvalue,
           &t->starttime,
           &t->vsize,
           &t->rss,
           &t->rlim,
           &t->startcode,
           &t->endcode,
           &t->startstack,
           &t->kstkesp,
           &t->kstkeip,
           &t->signal,
           &t->blocked,
           &t->sigignore,
           &t->sigcatch,
           &t->wchan,
           &t->nswap,
           &t->cnswap,
           &t->exit_signal,
           &t->processor,
           &t->rt_priority,
           &t->policy,
           &t->delayacct_blkio_ticks);

    /* Internal conversion */
    t->wr_rss      = (t->rss * wr_pagesize);
    t->wr_rss_hr   = human_readable_size(t->wr_rss);
    t->wr_utime_s  = (t->utime / wr_cpu_hz);
    t->wr_utime_ms = ((t->utime * 1000) / wr_cpu_hz);
    t->wr_stime_s  = (t->stime / wr_cpu_hz);
    t->wr_stime_ms = ((t->stime * 1000) / wr_cpu_hz);

    return t;
}

void wr_proc_free(struct wr_proc_task *t)
{
    free(t->wr_rss_hr);
    free(t);
    t = NULL;
}

void wr_proc_print(struct wr_proc_task *t)
{
    printf("pid         = %d\n", t->pid);
    printf("comm        = %s\n", t->comm);
    printf("state       = %c\n", t->state);
    printf("ppid        = %d\n", t->ppid);
    printf("pgrp        = %d\n", t->pgrp);
    printf("session     = %d\n", t->session);
    printf("tty_nr      = %d\n", t->tty_nr);
    printf("tpgid       = %d\n", t->tpgid);
    printf("flags       = %lu\n", t->flags);
    printf("minflt      = %lu\n", t->minflt);
    printf("cminflt     = %lu\n", t->cminflt);
    printf("majflt      = %lu\n", t->majflt);
    printf("cmajflt     = %lu\n", t->cmajflt);
    printf("utime       = %lu\n", t->utime);
    printf("stime       = %lu\n", t->stime);
    printf("cutime      = %ld\n", t->cutime);
    printf("cstime      = %ld\n", t->cstime);
    printf("priority    = %ld\n", t->priority);
    printf("nice        = %ld\n", t->nice);
    printf("num_threads = %ld\n", t->num_threads);
    printf("itrealvalue = %ld\n", t->itrealvalue);
    printf("starttime   = %lu\n", t->starttime);
    printf("vsize       = %lu\n", t->vsize);
    printf("rss         = %ld\n", t->rss);
    printf("rlim        = %lu\n", t->rlim);
    printf("startcode   = %lu\n", t->startcode);
    printf("endcode     = %lu\n", t->endcode);
    printf("startstack  = %lu\n", t->startstack);
    printf("kstkesp     = %lu\n", t->kstkesp);
    printf("kstkeip     = %lu\n", t->kstkeip);
    printf("signal      = %lu\n", t->signal);
    printf("blocked     = %lu\n", t->blocked);
    printf("sigignore   = %lu\n", t->sigignore);
    printf("sigcatch    = %lu\n", t->sigcatch);
    printf("wchan       = %lu\n", t->wchan);
    printf("nswap       = %lu\n", t->nswap);
    printf("cnswap      = %lu\n", t->cnswap);
    printf("exit_signal = %d\n", t->exit_signal);
    printf("processor   = %d\n", t->processor);
    printf("rt_priority = %lu\n", t->rt_priority);
    printf("policy      = %lu\n", t->policy);
    printf("delayacct_blkio_ticks = %llu\n", t->delayacct_blkio_ticks);

    fflush(stdout);
}
