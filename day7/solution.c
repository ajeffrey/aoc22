#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

enum debug {
	None = 0,
	Parser = 1,
	Part1 = 2,
	Part2 = 3
};

#define LINE_SIZE 256
#define DEBUG Part2
#define MAX_DEPTH 64
#define MAX_CHILDREN 64
#define MAX_NODES 256

typedef enum {
	T_FILE = 0,
	T_DIR = 1,
} node_type_t;

typedef struct node_t {
	struct node_t* parent;
	node_type_t type;
	char* name;
	union {
		struct {
			uint8_t n_children;
			struct node_t** children;
		};
		uint32_t size;
	};
} node_t;

struct timespec start, end;

void start_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
}

void end_timer() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    double diff = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec)) / 1e9;
    printf("= %.9fs\n", diff);
}

char* rdline(char* line, FILE* fp) {
	char* retval = fgets(line, LINE_SIZE - 1, fp);
	if(!retval) {
		return NULL;
	}

	char* chr = &line[strlen(line) - 1];
	while(*chr == '\n' || *chr == '\r') {
		*chr = 0x00;
		chr -= 1;
	}

	return retval;
}

node_t* parseInput(const char* path) {
	FILE* fp = fopen(path, "r");

	node_t* root;
	root = calloc(MAX_NODES, sizeof(node_t));
	root->parent = NULL;
	root->type = T_DIR;
	root->name = "/";
	root->n_children = 0;
	root->children = calloc(MAX_CHILDREN, sizeof(node_t*));
	node_t* cwd = root;
	uint32_t free = 1;
	char* chr;
	node_t* current;
	char* retval;

	char line[LINE_SIZE];
	retval = rdline(line, fp);
	while(retval != NULL) {
		DEBUG == Parser && printf("LINE: '%s'\n", line);
		if(line[0] == '$') {
			// ls
			if(line[2] == 'l' && line[3] == 's') {
					DEBUG == Parser && printf("listing...\n");
				while((retval = rdline(line, fp))) {
					DEBUG == Parser && printf("  '%s'\n", line);
					if(line[0] == '$') {
						break;
					}

					chr = &line[0];
					while(*chr != ' ') {
						chr += 1;
					}

					current = &root[free++];
					cwd->children[cwd->n_children] = current;
					cwd->n_children += 1;
					current->parent = cwd;
					current->name = strdup(chr + 1);

					if(line[0] == 'd') {
						current->type = T_DIR;
						current->n_children = 0;
						current->children = calloc(MAX_CHILDREN, sizeof(node_t*));
						DEBUG == Parser && printf("  added dir %s\n", current->name);
					} else {
						current->type = T_FILE;
						current->size = atoi(line);
						DEBUG == Parser && printf("  added file %s (%i)\n", current->name, current->size);
					}
				}

			} else if(line[2] == 'c' && line[3] == 'd') {
				if(line[5] == '/') {
					DEBUG == Parser && printf("cd /\n");
					cwd = root;

				} else if(line[5] == '.' && line[6] == '.') {
					if(cwd->parent == NULL) {
						DEBUG == Parser && printf("CANT cd .. - at root\n");
					} else {
						DEBUG == Parser && printf("cd ..\n");
						cwd = cwd->parent;
					}

				} else {
					uint8_t found = 0;
					DEBUG == Parser && printf("searching for '%s' in %i children\n", &line[5], cwd->n_children);
					for(int i = 0; i < cwd->n_children; i++) {
						if(cwd->children[i]->type == T_DIR && strcmp(cwd->children[i]->name, &line[5]) == 0) {
							cwd = cwd->children[i];
							found = 1;
							DEBUG == Parser && printf("entering dir %s\n", &line[5]);
							break;
						} else {
							DEBUG == Parser && printf("%s != %s\n", cwd->children[i]->name, &line[5]);
						}
					}
					if(!found) {
							DEBUG == Parser && printf("DIR '%s' NOT FOUND\n", &line[5]);
					}
				}
				
				retval = rdline(line, fp);
			}
		}
	}

	fclose(fp);

	return root;
}

uint32_t nodesize(node_t* node) {
	if(node->type == T_FILE) {
		return node->size;
	}

	uint32_t dirsize = 0;
	for(int i = 0; i < node->n_children; i++) {
		dirsize += nodesize(node->children[i]);
	}

	return dirsize;
}

uint32_t part1(node_t* root) {
	uint32_t total = 0;
	uint32_t size = nodesize(root);
	if(size <= 100000) {
		total += size;
	}
	for(int i = 0; i < root->n_children; i++) {
		if(root->children[i]->type == T_DIR) {
			total += part1(root->children[i]);
		}
	}

	return total;
}

uint32_t smallestdir(node_t* root, uint32_t gt) {
	uint32_t smallest = nodesize(root);
	if(smallest < gt) {
		smallest = INT_MAX;
	}
	uint32_t size = 0;
	for(int i = 0; i < root->n_children; i++) {
		if(root->children[i]->type == T_DIR) {
			size = smallestdir(root->children[i], gt);
			if(size < smallest && size >= gt) {
				smallest = size;
			}
		}
	}

	return smallest;
}

int part2(node_t* root) {
	uint32_t freespace = 70000000 - nodesize(root);
	uint32_t needed = 30000000 - freespace;
	printf("needed: %i\n", needed);
	return smallestdir(root, needed);
}

int main() {
	node_t* testInput = parseInput("test.txt");
	node_t* realInput = parseInput("data.txt");
	assert(part1(testInput) == 95437);
  start_timer();
	printf("part 1: %i\n", part1(realInput));
  end_timer();
	assert(part2(testInput) == 24933642);
  start_timer();
	printf("part 2: %i\n", part2(realInput));
  end_timer();
}