#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdnoreturn.h>

typedef uint64_t Cell;

typedef struct {
	Cell *tape;
	size_t tapesize;
	size_t ptr;
} bf_state;

typedef struct _llist {
	size_t n;
	struct _llist *next;
} list;

#define error(...) _error(__FILE__, __LINE__, __VA_ARGS__)
noreturn void _error(const char *file, int line, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "ERROR %s:%d: ", file, line);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

void *alloc(size_t size) {
	void *ret = calloc(1, size);
	if (!ret) {
		error("Error allocating.");
	}
	return ret;
}

#define INIT_TAPESIZE 512

bf_state init_state(void) {
	bf_state ret = {.tape = alloc(INIT_TAPESIZE * sizeof(Cell)), .tapesize = INIT_TAPESIZE, .ptr = 0};
	return ret;
}

void interpret(const char *bf, bf_state state) {
	char c;
	size_t i;
	list *stack = NULL;
	for (i = 0; (c = bf[i]); i++) {
		switch (c) {
			case '>':
				if (++state.ptr > state.tapesize-1) {
					// "realloc" except it zeroes everything else
					bf_state oldstate = state;
					state.tape = alloc(sizeof(Cell) * (state.tapesize *= 2));
					memcpy(state.tape, oldstate.tape, sizeof(Cell) * oldstate.tapesize);
					free(oldstate.tape);
				}
				break;
			case '<':
				// wrapping smenatics
				if ((state.ptr--) == 0) {
					state.ptr = state.tapesize-1;
				}
				break;
			case '+':
				state.tape[state.ptr]++;
				break;
			case '-':
				state.tape[state.ptr]--;
				break;
			case '.':
				putchar(state.tape[state.ptr]);
				break;
			case ',':
				state.tape[state.ptr] = getchar();
				break;
			// push the current code index onto the stack
			case '[':
				{
					list *old = stack;
					stack = alloc(sizeof(list));
					stack->n = i;
					stack->next = old;
				}
				break;
			case ']':
				if (!stack) {
					error("INTERNAL ERROR!  Mismatched loop (this should never be called");
				}
				if (state.tape[state.ptr]) {
					i = stack->n;
				} else {
					// pop off the top stack value, we're done with that loop
					list *old = stack->next;
					free(stack);
					stack = old;
				}
				break;
		}
	}
}

int main(int argc, char **argv) {
	if (argc == 1) {
		error("Required brainfuck code as command-line argument.");
	}

	bf_state state = init_state();
	interpret(argv[1], state);
	free(state.tape);

	return 0;
}
