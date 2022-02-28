EXE=um

compile:
	gcc -o $(EXE) -Wall -Wextra -O3 src/umachine.c src/main.c	\
	-lm -L./libs -lvector

run:
	./$(EXE) sandmark.umz

compile_debug:
	gcc -o $(EXE) -Wall -Wextra -g src/umachine.c src/main.c	\
	-lm -L./libs -lvector

run_debug:
	valgrind ./$(EXE) sandmark.umz

clean:
	rm -rf *.o $(EXE)
