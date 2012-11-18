
garner: main.o 
	g++ -Wall -O2 -o garner main.o -lgmp -lgmpxx

main.o: main.cpp garner.hpp gcde.hpp
	g++ -Wall -Wno-sign-compare -O2 -c main.cpp

clean:
	rm -f garner *.o
