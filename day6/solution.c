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

#define LINE_SIZE 4096
#define DEBUG None

typedef char* input_t;

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

	char* line = calloc(1, LINE_SIZE);
	fgets(line, LINE_SIZE - 1, fp);

	fclose(fp);

	return line;
}

int uniqlen(input_t input, int minlen) {
	size_t len = strlen(input);
	uint8_t dist[LINE_SIZE];
	dist[0] = 0;
	uint8_t found = 0;
	for(int i = 1; i < len; i++) {
		found = 0;
		if(DEBUG == Part1) {
			char* debug = strdup(input);
			debug[i + 1] = 0x00;
			int start = i - minlen + 1;
			if(start < 0) start = 0;
			printf("%i: %s\n", i, &debug[start]);
			free(debug);
		}
		for(int j = 1; j < minlen && (i - j) >= 0; j++) {
			if(input[i] == input[i - j]) {
				found = 1;
				dist[i] = j;
				DEBUG == Part1 && printf("%c (%i) == %c (%i) - dist %i\n", input[i], i, input[i - j], i - j, j);
				break;
			}
		}

		if(!found) {
			dist[i] = 0;
			if(i >= minlen - 1) {
				if(DEBUG == Part1) {
					for(int j = minlen - 1; j >= 0; j--) {
						printf("%i", dist[i - j]);
					}
					printf("\n");
				}
				for(int j = 1; j <= minlen; j++) {
					if(dist[i - minlen + j] != 0 && dist[i - minlen + j] < j) {
						DEBUG == Part1 && printf("dist[%i] = %i (<= %i)\n", j, dist[i - minlen + j], j);
						found = 1;
						break;
					}
				}
				if(!found) {
					DEBUG == Part1 && printf("%i good!\n", i);
					return i + 1;
				}
			}
		}
	}
	return -1;
}

int part1(input_t input) {
	return uniqlen(input, 4);
}

int part2(input_t input) {
	return uniqlen(input, 14);
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	assert(part1(testInput) == 7);
  start_timer();
	printf("part 1: %i\n", part1(realInput));
  end_timer();
	assert(part2(testInput) == 19);
  start_timer();
	printf("part 2: %i\n", part2(realInput));
  end_timer();
}