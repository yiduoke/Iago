all: iago bserver

forking: iago fserver

basic: iago bserver

fserver: forking_server.o pipe_networking.o
	gcc -o server forking_server.o pipe_networking.o

iago: iago.o pipe_networking.o
	gcc -o iago iago.o pipe_networking.o

bserver: basic_server.o pipe_networking.o
	gcc -o server basic_server.o pipe_networking.o

iago.o: iago.c pipe_networking.h
	gcc -c iago.c

basic_server.o: basic_server.c pipe_networking.h
	gcc -c basic_server.c

forking_server.o: forking_server.c pipe_networking.h
	gcc -c forking_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

clean:
	rm *.o
	rm *~
