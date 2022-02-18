EXE=um

compile:
	gcc -o $(EXE) -Wall -Wextra src/umachine.c src/main.c	\
	-lm -L./libs -lvector

run:
	./$(EXE) sandmark.umz

clean:
	rm -rf *.o $(EXE)
