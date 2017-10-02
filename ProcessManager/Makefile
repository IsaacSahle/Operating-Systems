.phony all:
all: rsi

rsi: rsi.c util.c
	gcc util.c rsi.c -lreadline -lhistory -o PMan

.PHONY clean:
clean:
	-rm -rf *.o *.exe
