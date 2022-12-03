all: project3

project3: project3.o
	gcc -o project3 project3.o

project3.o: project3.c
	gcc -c -g -Wall -pedantic project3.c

clean:
	rm project3 project3.o
