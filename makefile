all:
	gcc -o iago iago.c
clean:
	rm *~
	rm a.out
run: all
	./iago
