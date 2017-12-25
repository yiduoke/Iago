all:
	gcc -o iago iago.c

server: basic_server.o pipe_networking.o
	gcc -o server basic_server.o pipe_networking.o

basic_server.o: basic_server.c pipe_networking.h
	gcc -c basic_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

clean:
	rm *~
	rm a.out

run: all
	./iago
