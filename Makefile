CFLAGS = -Wall -Wextra -std=c11 -g3
SRC = brainfcuk.c

default: brainfcuk

brainfcuk: $(SRC)

clean:
	rm brainfcuk
