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
#define DEBUG Part1
#define MAX_RUCKSACKS 512

typedef struct rucksack_t {
	uint8_t size;
	char* left;
	char* right;
} rucksack_t;

typedef struct input_t {
	uint16_t total;
	rucksack_t rucksacks[MAX_RUCKSACKS];
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
	rucksack_t* current;

	char line[LINE_SIZE];
	while(fgets(line, LINE_SIZE - 1, fp)) {
		current = &input.rucksacks[input.total];
		current->size = strlen(line) / 2;
		current->left = strdup(line);
		current->right = &current->left[current->size];
		input.total += 1;
	}

	fclose(fp);

	return input;
}

uint32_t priority(char c) {
	return c >= 'a' ? (c - 'a' + 1) : (c - 'A' + 27);
}

int part1(input_t* input) {
	uint32_t score = 0;
	rucksack_t* current;
	char found = 0x00;
	for(int i = 0; i < input->total; i++) {
		current = &input->rucksacks[i];
		found = 0x00;
		for(int j = 0; j < current->size && !found; j++) {
			for(int k = 0; k < current->size && !found; k++) {
				if(current->left[j] == current->right[k]) {
					found = current->left[j];
					score += priority(found);
				}
			}
		}
	}
	return score;
}

int part2(input_t* input) {
	int  score = 0;
	uint8_t found = 0;
	for(int i = 0; i < input->total; i += 3) {
		found = 0;
		for(int j = 0; !found && j < input->rucksacks[i].size * 2; j++) {
			for(int k = 0; !found && k < input->rucksacks[i + 1].size * 2; k++) {
				if(input->rucksacks[i].left[j] == input->rucksacks[i + 1].left[k]) {
					for(int l = 0; !found && l < input->rucksacks[i + 2].size * 2; l++) {
						if(input->rucksacks[i].left[j] == input->rucksacks[i + 2].left[l]) {
							printf("%c", input->rucksacks[i].left[j]);
							found = 1;
							score += priority(input->rucksacks[i].left[j]);
						}
					}
				}
			}
		}
	}
	printf("\n");
	return score;
}

void freeInput(input_t* input) {
	for(int i = 0; i < input->total; i++) {
		free(input->rucksacks[i].left);
	}
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	assert(part1(&testInput) == 157);
  start_timer();
	printf("part 1: %i\n", part1(&realInput));
  end_timer();
	assert(part2(&testInput) == 70);
  start_timer();
	printf("part 2: %i\n", part2(&realInput));
  end_timer();
	freeInput(&testInput);
	freeInput(&realInput);
}