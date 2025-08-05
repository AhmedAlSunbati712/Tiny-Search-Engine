CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
OBJS = indexer.o
LIBS = ../common/common.a
LLIBS = ../libcs50/libcs50-given.a
L = ../libcs50
LL = ../common

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

TARGET = indexer

.PHONY: all valgrind test clean

all: indexer indextest

$(TARGET): $(OBJS) $(LIBS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJS): indexer.c $(LL)/index.h $L/hashtable.h $L/mem.h $L/file.h $L/webpage.h $(LL)/word.h
	$(CC) $(CFLAGS) -c $<

valgrind: $(TARGET)
	$(VALGRIND)

indextest: indextest.o $(LIBS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

indextest.o: indextest.c indexer.h $(LL)/index.h $L/file.h
	$(CC) $(CFLAGS) -c $<

test: indexer indextest
	bash -v testing.sh

clean:
	rm -f *.o
	rm -f vgcore.*
	rm -f ./indexer ./indextest