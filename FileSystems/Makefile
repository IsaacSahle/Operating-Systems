.phony all:
all: diskinfo disklist diskput diskget

diskinfo: diskinfo.c util.c
	gcc -Wall diskinfo.c util.c  -o diskinfo

disklist: disklist.c 
	gcc -Wall disklist.c -o disklist

diskput: diskput.c 
	gcc -Wall diskput.c -o diskput

diskget: diskget.c 
	gcc -Wall diskget.c -o diskget

.PHONY clean:
clean:
	-rm -rf *.o *.exe diskget diskinfo disklist diskput
