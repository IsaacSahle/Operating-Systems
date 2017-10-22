.phony all:
all: airline

airline: airline.c util.c
	gcc -Wall -pthread airline.c util.c -o ACS

.PHONY clean:
clean:
	-rm -rf *.o *.exe
