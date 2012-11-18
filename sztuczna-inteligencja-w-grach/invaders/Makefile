#fl  := -Wall -Wextra -ggdb ${SIG_INVADERS_OPT}
fl  := -Wall -Wextra -O2 -static -DNDEBUG ${SIG_INVADERS_OPT}
all : tags pt209223 $(variants)

tags: $(wildcard *.cc) $(wildcard *.hh)
	ctags $^

class.o : class.cc class.hh utils.hh 
	g++ $(fl) -c $< $(DEFS)

pt209223: pt209223.cc class.o gtp.hh memory.hh tree.hh utils.hh 
	g++ $(fl) -opt209223 pt209223.cc *.o

clean:
	rm -f *.o $(prg) core core.* tags

