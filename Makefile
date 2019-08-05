OBJ=opcoder.c

all:
	gcc -W -Wall -pedantic -std=c11 -o opcoder $(OBJ) -lkeystone -lstdc++ -lm
clean:
	rm -f opcoder
