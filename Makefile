CC      = gcc
CFLAGS  = -g -Wall
DEFS    =
INCDIR  = ./
OBJ     = wr.o proc.o

all: wr

wr: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -rf *.o *~ wr \#*

.c.o:
	$(CC) -c $(CFLAGS) $(DEFS) -I$(INCDIR) $<
