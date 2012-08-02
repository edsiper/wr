/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Monkey HTTP Daemon
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

#ifndef WR_H
#define WR_H

#define WR_FALSE     0
#define WR_TRUE      !WR_FALSE
#define WR_VERSION   "0.1"

/* Concurrency defaults */
#define WR_CONC_FROM 100
#define WR_CONC_TO   500
#define WR_CONC_STEP 20
#define WR_REQUESTS  500000

/* Kernel details */
int wr_pagesize;
int wr_cpu_hz;

/* ANSI Colors */
#define ANSI_BOLD "\033[1m"
#define ANSI_CYAN "\033[36m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_RED "\033[31m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_GREEN "\033[32m"
#define ANSI_WHITE "\033[37m"
#define ANSI_RESET "\033[0m"


/* Benchmark tool details */
#define BC_BIN "weighttp -t 10 -n %i -c %i %s %s"

#endif
