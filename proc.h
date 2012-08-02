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

#ifndef WR_PROC_H
#define WR_PROC_H

#define PROC_PID_SIZE      1024
#define PROC_STAT_BUF_SIZE 1024

/*
 * This 'stat' format omits the first two fields, due to the nature
 * of  sscanf(3) and whitespaces, programs with spaces in the name can
 * screw up when scanning the information.
 */
#define PROC_STAT_FORMAT "%c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %lu %lu %llu"

/* Our tast struct to read the /proc/PID/stat values */
struct wr_proc_task {
    int  pid;                  /* %d  */
    char comm[256];            /* %s  */
    char state;			       /* %c  */
    int ppid;			       /* %d  */
    int pgrp;			       /* %d  */
    int session;		       /* %d  */
    int tty_nr;			       /* %d  */
    int tpgid;			       /* %d  */
    unsigned long flags;	   /* %lu */
    unsigned long minflt;	   /* %lu */
    unsigned long cminflt;	   /* %lu */
    unsigned long majflt;	   /* %lu */
    unsigned long cmajflt;	   /* %lu */
    unsigned long utime;	   /* %lu */
    unsigned long stime; 	   /* %lu */
    long cutime;		       /* %ld */
    long cstime;		       /* %ld */
    long priority;		       /* %ld */
    long nice;			       /* %ld */
    long num_threads;		   /* %ld */
    long itrealvalue;		   /* %ld */
    unsigned long starttime;   /* %lu */
    unsigned long vsize;	   /* %lu */
    long rss;			       /* %ld */
    unsigned long rlim;		   /* %lu */
    unsigned long startcode;   /* %lu */
    unsigned long endcode;	   /* %lu */
    unsigned long startstack;  /* %lu */
    unsigned long kstkesp;	   /* %lu */
    unsigned long kstkeip;	   /* %lu */
    unsigned long signal;	   /* %lu */
    unsigned long blocked;	   /* %lu */
    unsigned long sigignore;   /* %lu */
    unsigned long sigcatch;	   /* %lu */
    unsigned long wchan;	   /* %lu */
    unsigned long nswap;	   /* %lu */
    unsigned long cnswap;	   /* %lu */
    int exit_signal;		   /* %d  */
    int processor;		       /* %d  */
    unsigned long rt_priority; /* %lu */
    unsigned long policy;	   /* %lu */
    unsigned long long delayacct_blkio_ticks; /* %llu */

    /* Internal conversion */
    long           wr_rss;            /* bytes = (rss * PAGESIZE)                 */
    char          *wr_rss_hr;         /* RSS in human readable format             */
    unsigned long  wr_utime_s;        /* seconds = (utime / _SC_CLK_TCK)          */
    unsigned long  wr_utime_ms;       /* milliseconds = ((utime * 1000) / CPU_HZ) */
    unsigned long  wr_stime_s;        /* seconds = (utime / _SC_CLK_TCK)          */
    unsigned long  wr_stime_ms;       /* milliseconds = ((utime * 1000) / CPU_HZ) */
};

struct wr_proc_task *wr_proc_stat(pid_t pid);
void wr_proc_free(struct wr_proc_task *t);
void wr_proc_print(struct wr_proc_task *t);

#endif
