# Piotr Truszkowski
#

ALL  = kalkulator
all: $(ALL)

%: %.asm
	nasm -f elf $@.asm
	ld -o $@ $@.o

clean:
	rm -f *.o $(ALL)
