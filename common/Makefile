CC = gcc
CFLAGS = -Wall -std=c11 -ggdb -I../libcs50
OBJS = pagedir.o index.o word.o query.o document.o
LIB = common.a
L = ../libcs50
LLIBS = ../libcs50/libcs50-given.a

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

pagedir.o: pagedir.c $L/webpage.h $L/mem.h
	$(CC) $(CFLAGS) -c $<

index.o: index.c $L/hashtable.h $L/counters.h $L/mem.h $L/file.h word.h
	$(CC) $(CFLAGS) -c $<

document.o: document.c $L/file.h $L/mem.h
	$(CC) $(CFLAGS) -c $<

query.o: query.c $L/bag.h $L/counters.h $L/hashtable.h $L/webpage.h $L/mem.h index.h document.h word.h
	$(CC) $(CFLAGS) -c $<

word.o: word.c $(L)/mem.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o