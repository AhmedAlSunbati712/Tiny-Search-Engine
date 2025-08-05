L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I../common
OBJS = crawler.o
LIBS = ../common/common.a
LLIBS = ../libcs50/libcs50-given.a

.PHONY: all clean test

all: crawler

# executable depends on object files
crawler: $(OBJS) $(LLIBS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) $(LIBS) -o crawler

# object files depend on include files
crawler.o: crawler.c $L/webpage.h $L/bag.h $L/hashtable.h ../common/pagedir.h
	$(CC) $(CFLAGS) -c crawler.c

test: 
	./testing.sh --verbose &> testing.out

clean:
	rm -f crawler.o crawler
	rm -f *~ *.o
	rm -rf *.dSYM
	rm -f $(wildcard vgcore.*)