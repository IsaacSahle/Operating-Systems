.phony all:
all: airline

airline: airline.c util.c
	gcc -Wall airline.c util.c -o airline

.PHONY clean:
clean:
	-rm -rf *.o *.exe
