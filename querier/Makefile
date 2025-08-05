CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
OBJS = querier.o
LIBS = ../common/common.a
LLIBS = ../libcs50/libcs50-given.a
L = ../libcs50
LL = ../common

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

TARGET = querier

.PHONY: all valgrind test clean

all: querier

$(TARGET): $(OBJS) $(LIBS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJS): querier.c $(LL)/query.h $(LL)/index.h $(LL)/word.h $L/bag.h $L/file.h $L/mem.h  
	$(CC) $(CFLAGS) -c $<

valgrind: 
	$(VALGRIND)

test: $(TARGET)
	bash -v testing.sh > testing.out 2>&1

clean:
	rm -f *.o
	rm -f vgcore.*
	rm -f querier