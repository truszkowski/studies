CXX         := g++
CXXFLAGS    := -Wall -Wno-sign-compare -c
LDFLAGS     := -Wall
SOURCES     := $(wildcard *.cpp)
MAINOBJECTS := $(subst .cpp,.o,$(shell grep -l main $(SOURCES)))
ALL         := $(subst .o,,$(MAINOBJECTS))
DEPENDS     := $(subst .cpp,.d,$(SOURCES))
ALLOBJECTS  := $(subst .cpp,.o,$(SOURCES))
OBJECTS	    := $(filter-out $(MAINOBJECTS),$(ALLOBJECTS)) 


all: $(DEPENDS) $(ALL)

$(DEPENDS) : %.d : %.cpp
	$(CXX) -MM $< > $@
	@echo -e: "\t"$(CXX) $(CXXFLAGS) $< >> $@

$(ALL) : % : %.o $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

-include $(DEPENDS)

clean:
	-rm -f *.o $(ALL) $(ALLOBJECTS) $(DEPENDS)
