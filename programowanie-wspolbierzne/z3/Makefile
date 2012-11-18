# Piotr Truszkoski

CC          := gcc
CFLAGS      := -Wall --pedantic -c
LDFLAGS     := -Wall --pedantic
SOURCES     := $(wildcard *.c)
HEADERS     := $(wildcard *.h)
OBJECTS	    := $(subst .c,.o,$(SOURCES))
ALL         := ematrix

$(ALL) : % : %.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJECTS) : %.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) $<

clean:
	-rm -f *.o $(ALL) $(OBJECTS)
