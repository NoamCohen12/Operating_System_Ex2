.PHONY: all clean

all: mynetcat ttt

mynetcat: mynetcat.c
	gcc mynetcat.c  -fprofile-arcs -ftest-coverage -Wall -g -o mynetcat

ttt: ttt.c
	gcc ttt.c -fprofile-arcs -ftest-coverage -Wall -g -o ttt

clean:
	rm -f *.o ttt mynetcat *.gcno *.gcda *.gcovs