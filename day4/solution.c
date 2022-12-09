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

#define LINE_SIZE 256
#define DEBUG None
#define MAX_PAIRS 1024

typedef struct pair_t {
	uint8_t a_start;
	uint8_t a_end;
	uint8_t b_start;
	uint8_t b_end;
} pair_t;

typedef struct input_t {
	uint16_t total;
	pair_t pairs[MAX_PAIRS];
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

input_t parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	input_t input;
	pair_t* current;
	uint32_t a_start, b_start, a_end, b_end;
	input.total = 0;

	char line[LINE_SIZE];
	while(fgets(line, LINE_SIZE - 1, fp)) {
		current = &input.pairs[input.total];
		sscanf(line, "%u-%u,%u-%u", &a_start, &a_end, &b_start, &b_end);
		current->a_start = a_start;
		current->a_end = a_end;
		current->b_start = b_start;
		current->b_end = b_end;
		input.total += 1;
	}

	fclose(fp);

	return input;
}

int part1(input_t* input) {
	uint16_t overlaps = 0;
	pair_t* current;
	for(int i = 0; i < input->total; i++) {
		current = &input->pairs[i];
		if((current->a_start >= current->b_start && current->a_end <= current->b_end) ||
			(current->b_start >= current->a_start && current->b_end <= current->a_end)) {
				overlaps += 1;
			}
	}

	return overlaps;
}

int part2(input_t* input) {
	uint16_t overlaps = 0;
	pair_t* current;
	for(int i = 0; i < input->total; i++) {
		current = &input->pairs[i];
		if((current->a_start <= current->b_end && current->a_end >= current->b_start) ||
			(current->b_start <= current->a_end && current->b_end >= current->a_start)) {
				overlaps += 1;
			}
	}

	return overlaps;
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	assert(part1(&testInput) == 2);
  start_timer();
	printf("part 1: %i\n", part1(&realInput));
  end_timer();
	assert(part2(&testInput) == 4);
  start_timer();
	printf("part 2: %i\n", part2(&realInput));
  end_timer();
}