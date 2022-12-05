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

#define CALORIE_BUFFER_SIZE 16
#define HIGHEST_CALORIE_BUFFER_SIZE 3
#define MAX_ELVES 1024
#define DEBUG None

struct timespec start, end;

void start_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
}

void end_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    double diff = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec)) / 1e9;
    printf("= %.9fs\n", diff);
}

typedef struct input_t {
	uint16_t total;
	uint32_t elves[MAX_ELVES];
} input_t;

input_t parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	input_t elves;
	memset(elves.elves, 0x00, sizeof(uint32_t) * MAX_ELVES);
	elves.total = 0;

	char line[CALORIE_BUFFER_SIZE];
	while(1) {
		if(!fgets(line, CALORIE_BUFFER_SIZE - 1, fp)) {
			elves.total += 1;
			break;
		}

		DEBUG == Parser && printf("line: %s (%lu)\n", line, strlen(line));

		// blank line - move off the current elf
		// but don't create a new one - might be EOF
		if(strlen(line) <= 1) {
			elves.total += 1;
			continue;
		}

		// non-blank line - new food
		elves.elves[elves.total] += atoi(line);
	}

	fclose(fp);

	if(DEBUG == Parser) {
		for(uint16_t i = 0; i < elves.total; i++) {
			printf("elf %u: %u\n", i, elves.elves[i]);
		}
	}

	return elves;
}

uint32_t part1(input_t* elf) {
	uint32_t highest = 0;
	for(uint16_t i = 0; i < elf->total; i++) {
		if(elf->elves[i] > highest) {
			highest = elf->elves[i];
		}
	}

	return highest;
}

void insertHighest(uint32_t* highest, uint32_t current) {
	uint32_t* maxDiff = &current;
	int32_t diff;
	DEBUG == Part2 && printf("checking %u against (%u, %u, %u)\n", current, highest[0], highest[1], highest[2]);
	for(int i = 0; i < HIGHEST_CALORIE_BUFFER_SIZE; i++) {
		diff = current - highest[i];
		if(diff > (int32_t)(current - *maxDiff)) {
			DEBUG == Part2 && maxDiff && printf("%i > %i\n", diff, (current - *maxDiff));
			maxDiff = &highest[i];
		}
	}

	DEBUG == Part2 && printf("%u -> %u\n", *maxDiff, current);
	*maxDiff = current;
	return;
}

uint32_t part2(input_t *elf) {
	uint32_t highest[HIGHEST_CALORIE_BUFFER_SIZE] = { 0, 0, 0 };
	uint32_t current;

	for(uint16_t i = 0; i < elf->total; i++) {
		insertHighest(highest, elf->elves[i]);
	}

	return highest[0] + highest[1] + highest[2];
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	assert(part1(&testInput) == 24000);
	start_timer();
	printf("highest: %i\n", part1(&realInput));
	end_timer();
	assert(part2(&testInput) == 45000);
	start_timer();
	printf("highest 3: %i\n", part2(&realInput));
	end_timer();
}