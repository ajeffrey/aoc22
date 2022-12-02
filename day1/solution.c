#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

enum debug {
	None = 0,
	Parser = 1,
	Part1 = 2,
	Part2 = 3
};

#define CALORIE_BUFFER_SIZE 16
#define HIGHEST_CALORIE_BUFFER_SIZE 3
#define DEBUG Part2

typedef struct food_t {
	int calories;
	struct food_t* next;
} food_t;

typedef struct elf_t {
	food_t* food;
	struct elf_t* next;
} elf_t;

food_t* mkFood() {
	food_t* food = (food_t*)malloc(sizeof(food_t));
	food->calories = 0;
	food->next = NULL;
	return food;
}

elf_t* mkElf() {
	elf_t* elf = (elf_t*)malloc(sizeof(elf_t));
	elf->next = NULL;
	elf->food = NULL;
	return elf;
}

elf_t* parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	elf_t* root = NULL;
	elf_t** cElf = &root;
	food_t** cFood = NULL;

	char* line = (char*)malloc(CALORIE_BUFFER_SIZE);
	while(fgets(line, CALORIE_BUFFER_SIZE - 1, fp)) {
		DEBUG == Parser && printf("line: %s (%lu)\n", line, strlen(line));

		// new elf
		if(!*cElf) {
			DEBUG == Parser && printf("new elf\n");
			*cElf = mkElf();
			cFood = &((*cElf)->food);
		}

		// blank line - move off the current elf
		// but don't create a new one - might be EOF
		if(strlen(line) <= 1) {
			DEBUG == Parser && printf("end of elf\n");
			cElf = &((*cElf)->next);
			continue;
		}

		// non-blank line - new food
		*cFood = mkFood();
		(*cFood)->calories = atoi(line);
		DEBUG == Parser && printf("new food (%i)\n", (*cFood)->calories);
		cFood = &(*cFood)->next;
	}

	free(line);
	fclose(fp);

	if(DEBUG == Parser) {
		for(elf_t* rElf = root; rElf && rElf->food; rElf = rElf->next) {
			printf("new elf\n");
			for(food_t* rFood = rElf->food; rFood; rFood = rFood->next) {
				printf("food: %i\n", rFood->calories);
			}
		}
	}

	return root;
}

void freeFood(food_t* food) {
	if(food) {
		freeFood(food->next);
		free(food);
	}
}

void freeElf(elf_t* elf) {
	if(elf) {
		freeElf(elf->next);
		freeFood(elf->food);
		free(elf);
	}
}

uint32_t part1(elf_t* root) {
	uint32_t highest = 0;
	uint32_t current;
	for(elf_t* celf = root; celf; celf = celf->next) {
		current = 0;
		for(food_t* cfood = celf->food; cfood; cfood = cfood->next) {
			current += cfood->calories;
		}

		if(current > highest) {
			highest = current;
		}
	}

	return highest;
}

void insertHighest(uint32_t* highest, uint32_t current) {
	uint32_t* maxDiff = NULL;
	int32_t diff;
	for(int i = 0; i < HIGHEST_CALORIE_BUFFER_SIZE; i++) {
		DEBUG == Part2 && printf("checking %u against (%u, %u, %u)\n", current, highest[0], highest[1], highest[2]);
		diff = current - highest[i];
		if((maxDiff == NULL && diff > 0) || (maxDiff != NULL && diff > (int32_t)(current - *maxDiff))) {
			DEBUG == Part2 && maxDiff && printf("%i > %i\n", diff, (current - *maxDiff));
			maxDiff = &highest[i];
		}
	}

	if(maxDiff) {
		DEBUG == Part2 && printf("%u -> %u\n", *maxDiff, current);
		*maxDiff = current;
	}
	return;
}

uint32_t part2(elf_t* root) {
	uint32_t highest[HIGHEST_CALORIE_BUFFER_SIZE] = { 0, 0, 0 };
	uint32_t current;
	for(elf_t* celf = root; celf; celf = celf->next) {
		current = 0;
		for(food_t* cfood = celf->food; cfood; cfood = cfood->next) {
			current += cfood->calories;
		}

		insertHighest(highest, current);
	}

	return highest[0] + highest[1] + highest[2];
}

int main() {
	elf_t* testInput = parseInput("test.txt");
	elf_t* realInput = parseInput("data.txt");
	assert(part1(testInput) == 24000);
	printf("highest: %i\n", part1(realInput));
	assert(part2(testInput) == 45000);
	printf("highest 3: %i\n", part2(realInput));
	freeElf(testInput);
	freeElf(realInput);
}