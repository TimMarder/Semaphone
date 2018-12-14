all: control.o main.o
	gcc -o control control.o
	gcc -o main main.o

control.o:
	gcc -c control.c

main.o:
	gcc -c main.c

clean:
	rm *.o
	rm *.txt
