
#ifndef SHORTESTPATH_H_   
#define SHORTESTPATH_H_

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

struct Qnode {
	uint64_t el;
	struct Qnode* next;
};

struct Q {
	struct Qnode* front;
	struct Qnode* rear;
};

struct Q* createQ();
struct Qnode* createQnode(uint64_t el);
void Print(struct Q* Q);
void EnQ(struct Q* Q, uint64_t el);
uint64_t DeQ(struct Q* Q);
int inQueue(struct Q* Q, uint64_t el);
uint64_t shortestPath(struct Graph* graph, uint64_t startNodeId, uint64_t endNodeId);

#endif
