all:
	gcc -o iago iago.c
clean:
	rm *~
run: all
	./iago
