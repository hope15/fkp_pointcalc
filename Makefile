# Makefile for 'pointcalc'
# @author hope15
# @date   01.12.2016

CC = gcc
DEFS = -D_XOPEN_SOURCE=500 -D_BSD_SOURCE
CFLAGS = -Wall -pedantic -g -std=c99 $(DEFS)
LDFLAGS =

OBJFILES = pointcalc.o

all: pointcalc

pointcalc: $(OBJFILES)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJFILES) pointcalc

.PHONY: all clean
