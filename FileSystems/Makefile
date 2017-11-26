.phony all:
all: diskinfo disklist diskput diskget

diskinfo: diskinfo.c util.c
	gcc -Wall diskinfo.c util.c  -o diskinfo

disklist: disklist.c util.c
	gcc -Wall disklist.c util.c -o disklist

diskput: diskput.c 
	gcc -Wall diskput.c -o diskput

diskget: diskget.c util.c 
	gcc -Wall diskget.c util.c -o diskget

.PHONY clean:
clean:
	-rm -rf *.o *.exe diskget diskinfo disklist diskput
