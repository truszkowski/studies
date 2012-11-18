CC          := g++
CFLAGS      := -Wall --pedantic -lpthread -c
LDFLAGS     := -Wall --pedantic -lpthread
SOURCES     := $(wildcard *.cpp)
MAINOBJECTS := $(subst .cpp,.o,$(shell grep -l main $(SOURCES)))
ALL         := $(subst .o,,$(MAINOBJECTS))
DEPENDS     := $(subst .cpp,.d,$(SOURCES))
ALLOBJECTS  := $(subst .cpp,.o,$(SOURCES))
OBJECTS	    := $(filter-out $(MAINOBJECTS),$(ALLOBJECTS)) 


all: $(DEPENDS) $(ALL)

$(DEPENDS) : %.d : %.cpp
	$(CC) -MM $< > $@
	@echo -e: "\t"$(CC) $(CFLAGS) $< >> $@

$(ALL) : % : %.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(DEPENDS)

clean:
	-rm -f *.o $(ALL) $(ALLOBJECTS) $(DEPENDS)
