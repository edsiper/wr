# Watch Resources

---

### Project moved to:

http://github.com/monkey/monkey-bench


This tool aims to work like a wrapper over weighttp to benchmark web
servers. It perform test under different levels of concurrency, as well
it takes snapshots of different metric such as user time, system time and
memory used.

The idea is taken from the G-Wab AB wrapper located under:

    http://gwan.com/source/ab.c

This tool is different than ab.c as it collect direct information from
the proc filesystem to gather the stats from each process instead of spawn
child processes like 'ps' to obtain the same.

For pending features refer to the TODO file.

Author
======
Written by Eduardo Silva <edsiper@gmail.com>
http://monkey-project.com
