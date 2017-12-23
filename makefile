all:
	gcc -o iago iago.c
clean:
	rm *~
	rm #~
run: all
	./iago
