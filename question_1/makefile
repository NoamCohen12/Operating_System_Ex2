.PHONY: all clean

all:ttt

ttt: ttt.o
	gcc ttt.o -o ttt

ttt.o: ttt.c
	gcc -c ttt.c -o ttt.o

clean:
	rm -f *.o ttt