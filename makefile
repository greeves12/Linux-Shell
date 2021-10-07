
clean:
	/bin/rm -f shell test1

all:	clean shell test1

shell:  shell.c
	gcc shell.c -o shell

test1:  test1.c
	gcc test1.c -o test1
