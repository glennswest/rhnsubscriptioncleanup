cleanup:  	cleanup.o cJson.o
	  	gcc -o cleanup cleanup.o cJson.o

cleanup.o:	cleanup.c
		gcc -c cleanup.c

cJson.o:	cJson.c
		gcc -c cJson.c


