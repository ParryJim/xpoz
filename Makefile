# Makefile (duh)

CC=gcc
CFLAGS=-I.

# demo command list method of graphics
demo1: demo1.o ~/xpoz.o ~/xpoz.h
	gcc -static -o demo1   demo1.o -lm ~/xpoz.o

# iquote your own home directory, or where ever you put xpoz.h
demo1.o: demo1.c ~/xpoz.h ~/xpoz.o
	gcc -c -o demo1.o -iquote /home/jjimparry demo1.c


~/xpoz.o: ~/xpoz.c ~/xpoz.h
	gcc -c -o ~/xpoz.o ~/xpoz.c


############ clean ##################
clean:
	-rm ~/xpoz.o demo1.o demo1
	
