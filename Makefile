CC            = gcc
CFLAGS        = -Wall -g
OBJS          = main.o log.o network.o signal.o memory.o
PROGRAM       = main

.SUFFIXES: .o .c

all:            $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM)

.c.o:
		$(CC) $(CFLAGS) -c $<

clean:
		rm -f *.o *~ $(PROGRAM)
