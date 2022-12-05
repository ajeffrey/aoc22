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
#define MAX_ROUNDS 4095

struct timespec start, end;

void start_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
}

void end_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    double diff = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec)) / 1e9;
    printf("= %.9fs\n", diff);
}

typedef struct round_t {
	uint8_t you;
	uint8_t me;
} round_t;

typedef struct input_t {
	uint16_t total;
	round_t rounds[MAX_ROUNDS];
} input_t;

input_t parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	input_t input;
	input.total = 0;
	memset(input.rounds, 0x00, sizeof(round_t) * MAX_ROUNDS);

	char line[LINE_SIZE];
	while(fgets(line, LINE_SIZE - 1, fp)) {
		input.rounds[input.total].you = (uint8_t)line[0] - 'A';
		input.rounds[input.total].me = (uint8_t)line[2] - 'X';
		input.total += 1;
	}

	if(DEBUG == Parser) {
		round_t* round;
		for(uint16_t i = 0; i < input.total; i++) {
			round = &input.rounds[i];
			printf("%i %i\n", round->you, round->me);
		}
	}

	fclose(fp);

	return input;
}

// TT
//   0 1 2 I
// 0 D W L
// 1 L D W
// 2 W L D
// U


// r = i - u + 1
inline int result(uint8_t you, uint8_t me) {
	return 3 * (((int8_t)me - (int8_t)you + 4) % 3);
} 

inline int price(uint8_t move) {
	return 1 + move;
}

inline int score(uint8_t you, uint8_t me) {
	return result(you, me) + price(me);
}

uint8_t rightmove(round_t* round) {
	return (round->me + round->you + 2) % 3;
}

int part1(input_t* input) {
	int total = 0;
	round_t* round;
	for(uint16_t i = 0; i < input->total; i++) {
		round = &input->rounds[i];
		DEBUG == Part1 && printf("(%i %i) %i (%i + %i)\n", round->you, round->me, score(round->you, round->me), result(round->you, round->me), price(round->me));
		total += score(round->you, round->me);
	}

	return total;
}

int part2(input_t* input) {
	int total = 0;
	round_t* round;
	for(uint16_t i = 0; i < input->total; i++) {
		round = &input->rounds[i];
		uint8_t move = rightmove(round);
		DEBUG == Part2 && printf("move = %i (%i, %i)\n", move, round->you, round->me);
		total += score(round->you, move);
	}

	return total;
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t realInput = parseInput("data.txt");
	printf("part 1: %i\n", part1(&testInput));
	assert(part1(&testInput) == 15);
	start_timer();
	printf("part 1: %i\n", part1(&realInput));
	end_timer();
	assert(part2(&testInput) == 12);
	start_timer();
	printf("part 2: %i\n", part2(&realInput));
	end_timer();
}