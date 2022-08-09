make: main.c ./methods.h
	gcc -o program main.c -lgslcblas -lm  -lgsl

run: main.c ./methods.h
	gcc -o program main.c -lgslcblas -lm  -lgsl
	./program

clean:
	rm ./program