all: pierscien proces

pierscien: pierscien.o err.o
	cc -Wall -o pierscien pierscien.o err.o

pierscien.o: pierscien.c konf.h err.h
	cc -Wall -c pierscien.c

proces: proces.o err.o
	cc -Wall -o proces proces.o err.o

proces.o: proces.c konf.h err.h
	cc -Wall -c proces.c

err.o: err.c err.h
	cc -Wall -c err.c

clean:
	rm -f *.o pierscien proces

