all:mync

mync: mync.o
	gcc mync.o -o mync

mync.o: mync.c
	gcc -c mync.c -o mync.o

clean:
	rm -f *.o mync