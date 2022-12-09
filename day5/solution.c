#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>
#include <time.h>

enum debug {
	None = 0,
	Parser = 1,
	Part1 = 2,
	Part2 = 3
};

enum parseMode {
	p_stacks = 0,
	p_moves = 1
};

#define LINE_SIZE 256
#define DEBUG None
#define MAX_MOVES 1024

typedef struct boxstack_t {
	char value;
	struct boxstack_t* next;
} boxstack_t;

typedef struct move_t {
	uint8_t from;
	uint8_t to;
	uint8_t count;
} move_t;

typedef struct input_t {
	uint8_t n_stacks;
	boxstack_t** stacks;
	uint16_t n_moves;
	move_t moves[MAX_MOVES];
} input_t;

struct timespec start, end;

void start_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
}

void end_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    uint32_t diff = ((end.tv_sec - start.tv_sec) * 1e6) + ((end.tv_nsec - start.tv_nsec) / 1e3);
    printf("= %i µs\n", diff);
}

void print_stacks(input_t* input) {
		boxstack_t* stacks[input->n_stacks];
		int i;
		uint8_t nonempty = 0;

		for(i = 0; i < input->n_stacks; i++) {
			stacks[i] = input->stacks[i];
		}

		do {
			nonempty = 0;
			for(i = 0; i < input->n_stacks; i++) {
				if(stacks[i]) {
					printf("[%c] ", stacks[i]->value);
					stacks[i] = stacks[i]->next;
					nonempty += 1;
				} else {
					printf("    ");
				}
			}
			printf("\n");
		} while(nonempty > 0);
}

input_t parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	input_t input;
	input.n_moves = 0;
	char parseMode = p_stacks;
	char value;
	int i;
	boxstack_t** cbox;
	boxstack_t* next;
	move_t* cmove;

	char line[LINE_SIZE];
	fgets(line, LINE_SIZE - 1, fp);
	input.n_stacks = strlen(line) / 4;
	DEBUG == Parser && printf("%u stacks\n", input.n_stacks);
	input.stacks = (boxstack_t**)calloc(input.n_stacks, sizeof(boxstack_t*));
	do {
		if(strlen(line) <= 1) {
			parseMode = p_moves;
			DEBUG == Parser && printf("switched to parsing moves\n");
			continue;
		}

		if(parseMode == p_stacks) {
			if(line[1] == '1') {
				continue;
			}

			for(int i = 0; i < input.n_stacks; i++) {
				value = line[1 + (i * 4)];
				if(value == ' ') continue;

				cbox = &input.stacks[i];
				while(*cbox) {
					cbox = &((*cbox)->next);
				}

				next = (boxstack_t*)malloc(sizeof(boxstack_t));
				next->value = value;
				next->next = NULL;
				*cbox = next;
			}

		} else {
			cmove = &input.moves[input.n_moves];
			int count, from, to;
			sscanf(line, "move %i from %i to %i", &count, &from, &to);
			cmove->count = count;
			cmove->from = from;
			cmove->to = to;
			input.n_moves += 1;
		}
	} while(fgets(line, LINE_SIZE - 1, fp));


	if(DEBUG == Parser) {
		print_stacks(&input);
		for(i = 0; i < input.n_moves; i++) {
			cmove = &input.moves[i];
			int count = cmove->count, from = cmove->from, to = cmove->to;
			printf("move %i from %i to %i\n", count, from, to);
		}
	}

	fclose(fp);

	return input;
}

int part1(input_t* input) {
	boxstack_t* cbox;
	move_t* cmove;
	if(DEBUG == Part1) {
		printf("init--\n");
		print_stacks(input);
	} 
	for(int i = 0; i < input->n_moves; i++) {
		cmove = &input->moves[i];
		for(int n = 0; n < cmove->count; n++) {
			cbox = input->stacks[cmove->from - 1];
			input->stacks[cmove->from - 1] = cbox->next;
			cbox->next = input->stacks[cmove->to - 1];
			input->stacks[cmove->to - 1] = cbox;
		}
		if(DEBUG == Part1) {
			printf("move %i-- %i => %i (%ix)\n", i + 1, (int)cmove->from, (int)cmove->to, (int)cmove->count);
			print_stacks(input);
		}
	}

	for(int i = 0; i < input->n_stacks; i++) {
		printf("%c", input->stacks[i]->value);
	}
	printf("\n");
	return 0;
}

int part2(input_t* input) {
	boxstack_t* startbox;
	boxstack_t* endbox;
	move_t* cmove;
	if(DEBUG == Part2) {
		printf("init--\n");
		print_stacks(input);
	} 
	for(int i = 0; i < input->n_moves; i++) {
		cmove = &input->moves[i];
		startbox = input->stacks[cmove->from - 1];
		endbox = startbox;
		for(int n = 1; n < cmove->count; n++) {
			endbox = endbox->next;
		}

		input->stacks[cmove->from - 1] = endbox->next;
		endbox->next = input->stacks[cmove->to - 1];
		input->stacks[cmove->to - 1] = startbox;

		if(DEBUG == Part2) {
			printf("move %i-- %i => %i (%ix)\n", i + 1, (int)cmove->from, (int)cmove->to, (int)cmove->count);
			print_stacks(input);
		}
	}

	for(int i = 0; i < input->n_stacks; i++) {
		printf("%c", input->stacks[i]->value);
	}
	printf("\n");
	return 0;
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	input_t realInput2 = parseInput("data.txt");
	// assert(part1(&testInput));
  start_timer();
	printf("part 1: %i\n", part1(&realInput));
  end_timer();
	// assert(part2(&testInput));
  start_timer();
	printf("part 2: %i\n", part2(&realInput2));
  end_timer();
}