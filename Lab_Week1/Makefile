CC=dcc
CFLAGS=-g

sim : sim.o Printers.o Queue.o
	$(CC) $(CFLAGS) -o sim sim.o Printers.o Queue.o

sim.o : sim.c Printers.h Queue.h
Printers.o : Printers.c Printers.h Queue.h
Queue.o : Queue.c Queue.h

clean :
	rm -f core sim sim.o Printers.o Queue.o

