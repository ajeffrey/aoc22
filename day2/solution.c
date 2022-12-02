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

#define LINE_SIZE 256
#define DEBUG Part2

typedef struct node_t {
	uint8_t you;
	uint8_t me;
  struct node_t* next;
} node_t;

node_t* mkNode(uint8_t you, uint8_t me) {
	node_t* node = (node_t*)malloc(sizeof(node_t));
	node->you = you;
	node->me = me;
	node->next = NULL;
	return node;
}

node_t* parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	node_t* root = NULL;
	node_t** cnode = &root;

	char* line = (char*)malloc(LINE_SIZE);
	while(fgets(line, LINE_SIZE - 1, fp)) {
		*cnode = mkNode((uint8_t)line[0] - 'A', (uint8_t)line[2] - 'X');
		cnode = &((*cnode)->next);
	}

	if(DEBUG == Parser) {
		for(node_t* node = root; node; node = node->next) {
			printf("%c %c\n", node->you, node->me);
		}
	}

	free(line);
	fclose(fp);

	return root;
}

void freeInput(node_t* input) {
  if(input) {
    freeInput(input->next);
    free(input);
  }
}

// TT
//   0 1 2 I
// 0 D W L
// 1 L D W
// 2 W L D
// U


// r = i - u + 1
int result(uint8_t you, uint8_t me) {
	return 3 * (((int8_t)me - (int8_t)you + 4) % 3);
} 

int price(uint8_t move) {
	return 1 + move;
}

int score(uint8_t you, uint8_t me) {
	return result(you, me) + price(me);
}

uint8_t rightmove(node_t* node) {
	return (node->me + node->you + 2) % 3;
}

int part1(node_t* root) {
	int total = 0;
	for(node_t* node = root; node; node = node->next) {
		DEBUG == Part1 && printf("%i (%i + %i)\n", score(node->you, node->me), result(node->you, node->me), price(node->me));
		total += score(node->you, node->me);
	}

	return total;
}

int part2(node_t* root) {
	int total = 0;
	for(node_t* node = root; node; node = node->next) {
		uint8_t move = rightmove(node);
		DEBUG == Part2 && printf("move = %i (%i, %i)\n", move, node->you, node->me);
		total += score(node->you, move);
	}

	return total;
}

int main() {
	node_t* testInput = parseInput("test.txt");
	node_t* realInput = parseInput("data.txt");
	assert(part1(testInput) == 15);
	printf("part 1: %i\n", part1(realInput));
	assert(part2(testInput) == 12);
	printf("part 2: %i\n", part2(realInput));
	freeInput(testInput);
	freeInput(realInput);
}