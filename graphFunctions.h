
#ifndef GRAPHFUNCTIONS_H_   
#define GRAPHFUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <ctype.h>
#include <inttypes.h>

struct Edge { //each of these is a connecting to a NODE
	struct Edge* next;
	uint64_t connectedId;
};

struct Node {
	struct Edge* head;
	uint64_t neighborCount; //same as the length of the linked list
	uint64_t nodeId;
	int color; //0 is white, 1 is grey, 2 is black
	uint64_t level;
};

struct Graph {
	struct Node* nodeArray;
	uint64_t size;
};

struct Graph* createGraph();
struct Node* createNode(uint64_t nodeId);
struct Edge* createEdge(uint64_t connectedId);
uint64_t nodeExist(struct Graph* graph, uint64_t nodeId);
int addNode(struct Graph* graph, uint64_t nodeId);
int remNode(struct Graph* graph, uint64_t nodeId);
int getNode(struct Graph* graph, uint64_t nodeId);
int edgeExist(struct Graph* graph, uint64_t nodeIndex, uint64_t connectedNodeId);
int getEdge(struct Graph* graph, uint64_t firstNodeId, uint64_t secondNodeId);
int addEdgeHelper(struct Graph* graph, uint64_t nodeIndex, uint64_t connectedNodeId);
int addEdge(struct Graph* graph, uint64_t firstNodeId, uint64_t secondNodeId);
int remEdgeHelper(struct Graph* graph, uint64_t nodeIndex, uint64_t connectedNodeId);
int remEdge(struct Graph* graph, uint64_t firstNodeId, uint64_t secondNodeId);
uint64_t* getNeighbors(struct Graph* graph, uint64_t nodeId);
void printGraph(struct Graph* graph);

#endif
