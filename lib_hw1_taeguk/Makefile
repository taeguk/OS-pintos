CC=gcc
CFLAGS=-g
OBJECTS=list.o hash.o bitmap.o test_list.o test_hash.o test_bitmap.o testlib.o
TARGET=testlib

all: $(TARGET)

list.o: list.c list.h
	$(CC) -c list.c $(CFLAGS)

hash.o: hash.c hash.h
	$(CC) -c hash.c $(CFLAGS)

bitmap.o: bitmap.c bitmap.h stdio.c limits.h round.h
	$(CC) -c bitmap.c $(CFLAGS)

test_list.o: test_list.c test_list.h list.h testlib.h
	$(CC) -c test_list.c $(CFLAGS)

test_hash.o: test_hash.c test_hash.h hash.h testlib.h
	$(CC) -c test_hash.c $(CFLAGS)

test_bitmap.o: test_bitmap.c test_bitmap.h bitmap.h testlib.h
	$(CC) -c test_bitmap.c $(CFLAGS)

testlib.o: testlib.c testlib.h test_list.h test_hash.h test_bitmap.h list.h hash.h bitmap.h
	$(CC) -c testlib.c $(CFLAGS)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS)

clean:
	rm -rf $(OBJECTS) $(TARGET)
