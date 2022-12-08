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
#define DEBUG Part2
#define MAX_LINES 256

typedef struct input_t {
	uint8_t width;
	uint8_t height;
	uint8_t* grid[MAX_LINES];
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
	input.width = 0;
	input.height = 0;

	char line[LINE_SIZE];
	while(fgets(line, LINE_SIZE - 1, fp)) {
		if(!input.width) {
			input.width = strlen(line) - 1;
		}

		input.grid[input.height] = (uint8_t*)calloc(input.width, sizeof(uint8_t));
		for(int i = 0; i < input.width; i++) {
			input.grid[input.height][i] = line[i] - '0';
		}
		input.height += 1;
	}

	fclose(fp);

	if(DEBUG == Parser) {
		printf("(%ix%i)\n", input.width, input.height);
		for(int y = 0; y < input.height; y++) {
			for(int x = 0; x < input.width; x++) {
				printf("%i", input.grid[y][x]);
			}
			printf("\n");
		}
	}

	return input;
}

uint8_t is_visible(input_t* input, uint8_t x, uint8_t y) {
	uint8_t found = 0;
	int8_t i;
	uint8_t height = input->grid[y][x];
	// left
	for(i = x - 1; i >= 0; i--) {
		if(input->grid[y][i] >= height) {
			found += 1;
			break;
		}
	}
	if(found == 0) {
		DEBUG == Part1 && printf("visible left\n");
		return 1;
	}
	// right
	for(i = x + 1; i < input->width; i++) {
		if(input->grid[y][i] >= height) {
			found += 1;
			break;
		}
	}
	if(found == 1) {
		DEBUG == Part1 && printf("visible right\n");
		return 1;
	}
	// up
	for(i = y - 1; i >= 0; i--) {
		if(input->grid[i][x] >= height) {
			found += 1;
			break;
		}
	}
	if(found == 2) {
		DEBUG == Part1 && printf("visible up\n");
		return 1;
	}
	// down
	for(i = y + 1; i < input->height; i++) {
		if(input->grid[i][x] >= height) {
			found += 1;
			break;
		}
	}
	if(found == 3) {
		DEBUG == Part1 && printf("visible down\n");
		return 1;
	}
	return 0;
}

uint32_t scenic_score(input_t* input, uint8_t x, uint8_t y) {
	uint32_t score[4] = {0,0,0,0};
	int8_t i;
	uint8_t height = input->grid[y][x];
	// left
	for(i = x - 1; i >= 0; i--) {
			score[0] += 1;
		if(input->grid[y][i] >= height) {
			break;
		}
	}
	// right
	for(i = x + 1; i < input->width; i++) {
			score[1] += 1;
		if(input->grid[y][i] >= height) {
			break;
		}
	}
	// up
	for(i = y - 1; i >= 0; i--) {
			score[2] += 1;
		if(input->grid[i][x] >= height) {
			break;
		}
	}
	// down
	for(i = y + 1; i < input->height; i++) {
			score[3] += 1;
		if(input->grid[i][x] >= height) {
			break;
		}
	}
	uint32_t total = score[0] * score[1] * score[2] * score[3];
	DEBUG == Part2 && printf("total: %i (%iu, %ir, %id, %il)\n", total, score[2], score[1], score[3], score[0]);
	return total;
}

int part1(input_t* input) {
	uint32_t visible = (input->height * 2) + (input->width * 2) - 4;

	for(uint8_t y = 1; y < (input->height - 1); y++) {
		for(uint8_t x = 1; x < (input->width - 1); x++) {
			DEBUG == Part1 && printf("testing %i [%i,%i]\n", input->grid[y][x], x, y);
			if(is_visible(input, x, y)) {
				visible += 1;
			}
		}
	}

	return visible;
}

int part2(input_t* input) {
	uint32_t max_scenic_score = 0;
	uint32_t new_scenic_score;
	for(uint8_t y = 1; y < (input->height - 1); y++) {
		for(uint8_t x = 1; x < (input->width - 1); x++) {
			DEBUG == Part2 && printf("testing %i [%i,%i]\n", input->grid[y][x], x, y);
			new_scenic_score = scenic_score(input, x, y);
			if(new_scenic_score > max_scenic_score) {
				max_scenic_score = new_scenic_score;
			}
		}
	}
	return max_scenic_score;
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	assert(part1(&testInput) == 21);
  start_timer();
	printf("part 1: %i\n", part1(&realInput));
  end_timer();
	assert(part2(&testInput) == 8);
  start_timer();
	printf("part 2: %i\n", part2(&realInput));
  end_timer();
}