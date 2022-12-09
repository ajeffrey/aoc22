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
#define MAX_MOVES 2048
#define GRID_SIZE 512
#define DRAW_SIZE 32
#define N_KNOTS 10

typedef struct coord_t {
	int32_t x;
	int32_t y;
} coord_t;

typedef struct move_t {
	coord_t dir;
	uint8_t steps;
} move_t;

typedef struct input_t {
	uint16_t n_moves; 
	move_t* moves;
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

char dirchar(coord_t c) {
	if(c.y > 0) return 'U';
	if(c.x > 0) return 'R';
	if(c.y < 0) return 'D';
	if(c.x < 0) return 'L';
	return '?';
}

input_t parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	input_t input;
	input.n_moves = 0;
	input.moves = calloc(MAX_MOVES, sizeof(move_t));
	move_t* c;

	char line[LINE_SIZE];
	while(fgets(line, LINE_SIZE - 1, fp)) {
		c = &input.moves[input.n_moves];
		c->steps = atoi(&line[2]);
		switch(line[0]) {
			case 'U': c->dir = (coord_t){ 0, 1 }; break;
			case 'R': c->dir = (coord_t){ 1, 0 }; break;
			case 'D': c->dir = (coord_t){ 0, -1 }; break;
			case 'L': c->dir = (coord_t){ -1, 0 }; break;
		}
		input.n_moves += 1;
	}

	fclose(fp);

	if(DEBUG == Parser) {
		for(uint32_t i = 0; i < input.n_moves; i++) {
			c = &input.moves[i];
			printf("%i: %c [%i,%i] %i\n", i, dirchar(c->dir), c->dir.x, c->dir.y, c->steps);
		}
	}

	return input;
}

uint8_t* visited_at(uint8_t* visited, int32_t x, int32_t y) {
	return &visited[((y + GRID_SIZE / 2) * GRID_SIZE) + x + GRID_SIZE / 2];
}

void follow(coord_t* head, coord_t* tail, uint8_t* visited) {
	coord_t diff;
	coord_t absdiff;
	diff.x = head->x - tail->x;
	diff.y = head->y - tail->y;
	int8_t dirx, diry;
	absdiff.x = abs(diff.x);
	absdiff.y = abs(diff.y);
	dirx = diff.x > 0 ? 1 : -1;
	diry = diff.y > 0 ? 1 : -1;
	int newvisit = 0;
	int oldvisit = 0;

	// >1 king's move away
	if(absdiff.x > 1 || absdiff.y > 1) {
		if(absdiff.x > absdiff.y) {
			while(head->x - tail->x != dirx) {
				tail->x += dirx;
				if(tail->y != head->y) {
					tail->y += diry;
				}
				if(visited) {
					uint8_t* loc = visited_at(visited, tail->x, tail->y);
					if(*loc) {
						oldvisit += 1;
					} else {
						newvisit += 1;
						*loc = 1;
					}
				}
			}
		} else {
			while(head->y - tail->y != diry) {
				tail->y += diry;
				if(tail->x != head->x) {
					tail->x += dirx;
				}
				if(visited) {
					uint8_t* loc = visited_at(visited, tail->x, tail->y);
					if(*loc) {
						oldvisit += 1;
					} else {
						newvisit += 1;
						*loc = 1;
					}
				}
			}
		}
	}

	if(visited && DEBUG == Part2) printf("visited %i new, %i old\n", newvisit, oldvisit);
}

int part1(input_t* input) {
	coord_t head = { 0, 0 };
	coord_t tail = { 0, 0 };
	move_t* c;
	uint8_t visited[GRID_SIZE * GRID_SIZE];
	memset(visited, 0x00, GRID_SIZE * GRID_SIZE * sizeof(uint8_t));
	*visited_at(visited, 0, 0) = 1;

	for(uint16_t i = 0; i < input->n_moves; i++) {
		c = &input->moves[i];
		head.x += c->steps * c->dir.x;
		head.y += c->steps * c->dir.y;
		follow(&head, &tail, visited);
		DEBUG == Part1 && printf("after %c %i: H[%i,%i] T[%i,%i]\n", dirchar(c->dir), c->steps, head.x, head.y, tail.x, tail.y);
	}
	uint32_t total_visited = 0;
	for(int y = GRID_SIZE - 1; y >= 0; y--) {
		for(int x = 0; x < GRID_SIZE; x++) {
			total_visited += visited[(y * GRID_SIZE) + x];
		}
	}

	return total_visited;
}

void draw_rope(coord_t* knots) {
	coord_t* c;
	int32_t minx = knots[0].x, maxx = knots[0].x, miny = knots[0].y, maxy = knots[0].y;
	for(int i = 1; i < N_KNOTS; i++) {
		c = &knots[i];
		if(c->x < minx) minx = c->x;
		if(c->y < miny) miny = c->y;
		if(c->x > maxx) maxx = c->x;
		if(c->y > maxy) maxy = c->y;
	}

	int8_t w = maxx - minx + 1;
	int8_t h = maxy - miny + 1;

	char grid[w * h];
	memset(grid, '.', w * h * sizeof(char));
	for(int i = N_KNOTS - 1; i >= 0; i--) {
		c = &knots[i];
		grid[((c->y - miny) * w) + (c->x - minx)] = i == 0 ? 'H' : (i + '0');
	}

	printf("@ %i,%i [%ix%i]\n", minx, miny, w, h);
	for(int8_t y = h - 1; y >= 0; y--) {
		for(int8_t x = 0; x < w; x++) {
			printf("%c", grid[y * w + x]);
		}
		printf("\n");
	}
	printf("\n");
}

int part2(input_t* input) {
	coord_t knots[N_KNOTS];
	coord_t* head = &knots[0];
	coord_t* tail = &knots[N_KNOTS - 1];
	for(int i = 0; i < N_KNOTS; i++) {
		knots[i].x = 0;
		knots[i].y = 0;
	}
	move_t* c;
	uint8_t visited[GRID_SIZE * GRID_SIZE];
	memset(visited, 0x00, GRID_SIZE * GRID_SIZE * sizeof(uint8_t));
	*visited_at(visited, 0, 0) = 1;

	for(uint16_t i = 0; i < input->n_moves; i++) {
		c = &input->moves[i];
		for(uint16_t j = 0; j < c->steps; j++) {
			head->x += c->dir.x;
			head->y += c->dir.y;
			for(int j = 1; j < N_KNOTS; j++) {
				follow(&knots[j - 1], &knots[j], &knots[j] == tail ? visited : NULL);
			}
		}
		if(DEBUG == Part2) {
			printf("after %c %i\nH[%i,%i]", dirchar(c->dir), c->steps, head->x, head->y);
			for(int i = 1; i < N_KNOTS; i++) {
				printf(" %c[%i,%i]", i + '0', knots[i].x, knots[i].y);
			}
			printf("\n");
			draw_rope(knots);
		}
	}
	uint32_t total_visited = 0;
	for(int y = GRID_SIZE - 1; y >= 0; y--) {
		for(int x = 0; x < GRID_SIZE; x++) {
			total_visited += visited[(y * GRID_SIZE) + x];
		}
	}

	if(DEBUG == Part2) {
		for(int y = DRAW_SIZE/2; y > -DRAW_SIZE/2; y--) {
			for(int x = -DRAW_SIZE/2; x < DRAW_SIZE/2; x++) {
				if(x == 0 && y == 0) {
					printf("s");
				} else {
					printf("%c", *visited_at(visited, x, y) ? '#' : '.');
				}
			}
			printf("\n");
		}
	}

	return total_visited;
}

int main() {
	input_t testInput = parseInput("test.txt");
	input_t testInput2 = parseInput("test2.txt");
	input_t realInput = parseInput("data.txt");
	assert(part1(&testInput) == 13);
  start_timer();
	printf("part 1: %i\n", part1(&realInput));
  end_timer();
	assert(part2(&testInput2) == 36);
  start_timer();
	printf("part 2: %i\n", part2(&realInput));
  end_timer();
}