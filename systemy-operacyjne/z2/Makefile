ALL := filesystem

filesystem: filesystem.o struct.o
	gcc -Wall --pedantic `pkg-config fuse --libs` -lpthread -lz -o filesystem filesystem.o struct.o

filesystem.o: filesystem.c struct.h
	gcc -Wall --pedantic `pkg-config fuse --cflags` -c filesystem.c 

struct.o: struct.c struct.h
	gcc -Wall --pedantic -c struct.c 

clean:
	rm -f *.o $(ALL)

