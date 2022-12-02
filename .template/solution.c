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
#define DEBUG None

typedef struct node_t {
  struct node_t* next;
} node_t;

node_t* mkNode() {
	node_t* node = (node_t*)malloc(sizeof(node_t));
	node->next = NULL;
	return node;
}

node_t* parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	node_t* root = NULL;
	node_t** cnode = &root;

	char* line = (char*)malloc(LINE_SIZE);
	while(fgets(line, LINE_SIZE - 1, fp)) {
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

int part1(node_t* root) {
	return 0;
}

int part2(node_t* root) {
	return 0;
}

int main() {
	node_t* testInput = parseInput("test.txt");
	node_t* realInput = parseInput("data.txt");
	assert(part1(testInput));
	printf("part 1: %i\n", part1(realInput));
	assert(part2(testInput));
	printf("part 2: %i\n", part2(realInput));
	freeInput(testInput);
	freeInput(realInput);
}