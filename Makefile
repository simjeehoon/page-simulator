simulator: ll.o main.o maker
	gcc -o simulator ll.o main.o

maker: util.o ll.o
	gcc -o maker util.o ll.o

util.o: util.c
	gcc -c -o util.o util.c

ll.o: ll.c ll.h
	gcc -c -o ll.o ll.c

main.o: main.c
	gcc -c -o main.o main.c

clean:
	rm maker simulator *.o
