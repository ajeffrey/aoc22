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

typedef struct input_t {
} input_t;

struct timespec start, end;

void start_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
}

void end_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    double diff = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec)) / 1e9;
    printf("= %.9fs\n", diff);
}

input_t parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	input_t input;

	char line[LINE_SIZE];
	while(fgets(line, LINE_SIZE - 1, fp)) {
	}

	fclose(fp);

	return input;
}

int part1(input_t* input) {
	return 0;
}

int part2(input_t* input) {
	return 0;
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	assert(part1(&testInput));
  start_timer();
	printf("part 1: %i\n", part1(&realInput));
  end_timer();
	assert(part2(&testInput));
  start_timer();
	printf("part 2: %i\n", part2(&realInput));
  end_timer();
}