

shell:mysh.c strmanip.o strmanip.h
	gcc strmanip.o mysh.c -o mysh

strmanip.o:strmanip.c strmanip.h
	gcc -c strmanip.c

clean:
	rm -f mysh *.o
