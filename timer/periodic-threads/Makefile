# Makefile for periodic threads example programs
# Chris Simmonds
# chris@2net.co.uk

CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall -g -lpthread
PROGS=itimer timer timerfd

all: $(PROGS)

timerfd: timerfd.c
	$(CC) $(CFLAGS) -lrt -o $@ $^

timer: timer.c
	$(CC) $(CFLAGS) -lrt -o $@ $^

itimer: itimer.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o
	rm -f $(PROGS)

install:
	cp $(PROGS) $(TARGETDIR)/bin
