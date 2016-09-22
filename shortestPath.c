#define __STDC_FORMAT_MACROS
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include "graphFunctions.h"

struct Qnode {
	uint64_t el;
	struct Qnode* next;
};

struct Q {
	struct Qnode* front;
	struct Qnode* rear;
};

struct Q* createQ() {
	struct Q* newQ = (struct Q*)malloc(sizeof(struct Q));
	newQ->front = NULL;
	newQ->rear = NULL;

	return newQ;
}

struct Qnode* createQnode(uint64_t el) {
	struct Qnode* newQnode = (struct Qnode*)malloc(sizeof(struct Qnode));
	newQnode->el = el;
	newQnode->next = NULL;

	return newQnode;
}

void Print(struct Q* Q) {
	struct Qnode* temp = Q->front;
	printf("\n");

	if (temp == NULL) {
		printf("empty Q\n");
	}

	while(temp != NULL) {
		printf("%" PRIu64 "-> ",temp->el);
		temp = temp->next;
	}
	printf("\n");
}

void EnQ(struct Q* Q, uint64_t el) {
	struct Qnode* newQnode = createQnode(el);

	if (Q->front == NULL) {
		//means this is first val in Q
		Q->front = newQnode;
	}
	else {
		Q->rear->next = newQnode;
	}

	Q->rear = newQnode;

	printf("que enqued\n");
	Print(Q);
	return;
}

uint64_t DeQ(struct Q* Q) {
	uint64_t retEl;
	struct Qnode* temp;

	if (Q->front == NULL) {
		//empty Queue
		return -1;
	}

	else {
		retEl = Q->front->el;
		temp = Q->front;
		Q->front = temp->next;
		free(temp);
	}

	printf("que dequed\n");
	Print(Q);
	return retEl;
}

int inQueue(struct Q* Q, uint64_t el) {
	struct Qnode* start = Q->front;
	while(start != NULL) {
		if (start->el == el) {
			return 1;
		}

		start = start->next;
	}

	return 0;
}

bool QExist(struct Q* Q) {
	if (Q->front != NULL) {
		return true;
	}

	else {
		return false;
	}
}


uint64_t shortestPath(struct Graph* graph, uint64_t startNodeId, uint64_t endNodeId) {
	//check to see whether the start/end node exists
	uint64_t startNodeIndex = nodeExist(graph, startNodeId);
	uint64_t endNodeIndex = nodeExist(graph, endNodeId);

	if (startNodeIndex == endNodeIndex) {
		return 204;
	}

	else if (startNodeIndex != -1 && endNodeIndex != -1) {
		//level for the first node = 0
		graph->nodeArray[startNodeIndex].level = 0;

		printf("we begin the function.\nThe startNodeId is %" PRIu64 " and the endNodeId is %" PRIu64 "\n", startNodeId, endNodeId);
		printf("the indices are %" PRIu64 " and %" PRIu64 " resp.", startNodeIndex, endNodeIndex);

		//create new Queue and enqueue start node
		struct Q* Q = createQ();
		EnQ(Q, startNodeId);

		//while Q has elements
		do {
			//get the node to test by dequeueing Q
			uint64_t currNodeId = DeQ(Q);
			printf("Now we are in the do-while loop\nThe currNodeId is %" PRIu64 "\n", currNodeId);
			//find dequeued node Index
			uint64_t currNodeIndex = nodeExist(graph, currNodeId);
			printf("The currNodeIndex is %" PRIu64 "\n", currNodeIndex);
			//get number of neighbors of currNode
			uint64_t numNeighbors = graph->nodeArray[currNodeIndex].neighborCount;
			printf("The numNeighbors of currentNode is %" PRIu64 "\n", numNeighbors);
			//then get the array of neighbors
			uint64_t* neighborArray = getNeighbors(graph, currNodeIndex);

			/*
			int* neighborArrayInt = (int*)malloc(sizeof(int)*numNeighbors);
			for (int i = 0; i < numNeighbors; i++) {
				neighborArrayInt[i] = neighborArray[i];
			}
			*/

			printf("here are the neighbors: [");
			for (uint64_t i = 0; i < numNeighbors; i++) {
				printf("%" PRIu64 " ", neighborArray[i]);
			}
			printf("]\n");

			printf("checking nodeArray[1] and nodeArrayInt[1]: %" PRIu64 " %i\n", neighborArray[1], 66);
			
			for(uint64_t i = 0; i < numNeighbors; i++) {
				//iterate through neighbor array
				uint64_t currNeighbor = neighborArray[i];
				printf("the current neighbor id is %" PRIu64 " and its index is %" PRIu64 "\n", currNeighbor, i);
				
				//FIRST check whether this neighbor is the node we're looking for
				if (endNodeId == currNeighbor) {
					printf("found it");
					return (graph->nodeArray[currNodeIndex].level + 1);
				}

				//if this is not the node we're looking for
				//get index of neighbor in graph
				uint64_t neighborIndex = nodeExist(graph, currNeighbor);
				printf("The index of the current neighbor in graph is %" PRIu64 "\n", neighborIndex);

				//check if this node has been traversed
				if (graph->nodeArray[neighborIndex].color == 2) {
					//do nothing
					printf("The color is black so we are skipping node # %" PRIu64 "\n", currNeighbor);
				}

				else if (graph->nodeArray[neighborIndex].color == 0) {
					//this means the node is untraversed
					//change the level of this node
					graph->nodeArray[neighborIndex].level = (graph->nodeArray[currNodeIndex].level + 1);
					graph->nodeArray[neighborIndex].color = 1;
					printf("setting level of %" PRIu64 " to %" PRIu64 "\n", currNeighbor, graph->nodeArray[currNodeIndex].level + 1);
					//now EnQ this node to be traversed
					EnQ(Q, currNeighbor);
				}

				else if (graph->nodeArray[neighborIndex].color == 1) {
					if (inQueue(Q, currNeighbor) == 0) {
						printf("enquing %" PRIu64 " because we have seen it before but not traversed\n", currNeighbor);
						EnQ(Q, currNeighbor);
					}

					else {
						printf("neighbor is grey and already enqueued\n");
					}
				}

				else {
					printf("I did fucking nothing\n");
				}
			}

			graph->nodeArray[currNodeIndex].color = 2;
			printf("Changed color of node in graph index %" PRIu64 "\n", currNodeIndex);
			Print(Q);
		} while(Q->front != NULL);

		return 204;
	} 

	else {
		printf("else 33");
		return -100;
	}
}
