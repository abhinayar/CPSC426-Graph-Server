#define __STDC_FORMAT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <ctype.h>
#include <inttypes.h>
#include "graphFunctions.h"
#include "shortestPath.h"

struct Graph* createGraph() {
	struct Graph* newGraph = (struct Graph*)malloc(sizeof(struct Graph));
	newGraph->nodeArray = NULL;
	newGraph->size = 0;

	return newGraph;
}

struct Node* createNode(uint64_t nodeId){
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->head = NULL;
	newNode->neighborCount = 0;
	newNode->nodeId = nodeId;
	newNode->color = 0;
	newNode->level = -1;

	return newNode;
}

struct Edge* createEdge(uint64_t connectedId) {
	struct Edge* newConnection = (struct Edge*)malloc(sizeof(struct Edge));
	newConnection->next = NULL;
	newConnection->connectedId = connectedId;

	return newConnection;
}

uint64_t nodeExist(struct Graph* graph, uint64_t nodeId) {
	uint64_t graphSize = graph->size;
	if (graphSize == 0) {
		//graph is empty return -1 to indicate no duplicate
		return -1;
	}

	else {
		for (int i=0; i<graphSize; i++) {
			if (graph->nodeArray[i].nodeId == nodeId) {
				return i;
			}
		}

		return -1;
	}
}

int addNode(struct Graph* graph, uint64_t nodeId) {
	uint64_t graphSize = graph->size;
	int isAlreadyNode = nodeExist(graph, nodeId);

	if (isAlreadyNode == -1) {
		uint64_t newGraphSize = graphSize + 1;
		struct Node* newNode = createNode(nodeId);
		graph->nodeArray = realloc(graph->nodeArray, newGraphSize * sizeof(struct Node));
		graph->nodeArray[graphSize] = *newNode;
		graph->size = newGraphSize;

		return 200;
	}

	else {
		return 204;
	}
}

int remNode(struct Graph* graph, uint64_t nodeId) {
	uint64_t graphSize = graph->size;
	int isAlreadyNode = nodeExist(graph, nodeId);

	if (isAlreadyNode!=-1) {
		uint64_t nodeIndex = isAlreadyNode; //rename the var

		if (graph->nodeArray[nodeIndex].head == NULL) {
			//if no connections don't need to free any
			//do nothing
		}

		else {
			printf("\nthere are connections on node %" PRIu64 "", nodeId);
			struct Edge* head = graph->nodeArray[nodeIndex].head;
			struct Edge* temp;

			uint64_t nodeIndex = nodeExist(graph, head->connectedId);
			remEdgeHelper(graph, nodeIndex, nodeId);
		
			while(head->next != NULL) {
				temp = head;
				uint64_t nodeIndex = nodeExist(graph, head->next->connectedId);
				remEdgeHelper(graph, nodeIndex, nodeId);
				head = head->next;
				free(temp);
			}

			free(head);
		}

		graph->nodeArray[nodeIndex].nodeId = -1;
		graph->nodeArray[nodeIndex] = graph->nodeArray[graphSize-1];
		graph->nodeArray = realloc(graph->nodeArray, (graphSize-1) * sizeof(struct Node));

		graph->size = graphSize-1;

		return 200;
	}

	else {
		return 400;
	}
}

int getNode(struct Graph* graph, uint64_t nodeId) {
	uint64_t nodeIndex = nodeExist(graph, nodeId);
	if (nodeIndex != -1) {
		return 202;
	}

	else {
		return 203;
	}
}

int edgeExist(struct Graph* graph, uint64_t nodeIndex, uint64_t connectedNodeId) {
	struct Edge* temp = graph->nodeArray[nodeIndex].head;

	while(temp) {
		if (temp->connectedId == connectedNodeId) {
			return 1;
		}

		temp = temp->next;
	}

	return 0;

}

int getEdge(struct Graph* graph, uint64_t firstNodeId, uint64_t secondNodeId) {
	if (firstNodeId == secondNodeId) {
		return 400;
	}

	int firstNodeIndex = nodeExist(graph, firstNodeId);
	int secondNodeIndex = nodeExist(graph, secondNodeId);
	if (firstNodeIndex == -1 || secondNodeIndex == -1) {
		return 400;
	}

	int retVal = edgeExist(graph, firstNodeIndex, secondNodeId);

	if (retVal == 1) {
		return 205;
	}

	else {
		return 406;
	}
}

int addEdgeHelper(struct Graph* graph, uint64_t nodeIndex, uint64_t connectedNodeId) {
	struct Edge* newEdge = createEdge(connectedNodeId);

	if (graph->nodeArray[nodeIndex].head == NULL) {
		graph->nodeArray[nodeIndex].head = newEdge;
	}

	else {
		int doesEdgeExist = edgeExist(graph, nodeIndex, connectedNodeId);

		if (doesEdgeExist == 0) {
			struct Edge* temp = graph->nodeArray[nodeIndex].head;

			while(temp->next != NULL) {
				temp = temp->next;
			}

			temp->next = newEdge;
		}	

		else {
			return 204;
		}
	}

	graph->nodeArray[nodeIndex].neighborCount++;

	return 1;
}

int addEdge(struct Graph* graph, uint64_t firstNodeId, uint64_t secondNodeId) {
	int firstNodeIndex = nodeExist(graph, firstNodeId);
	int secondNodeIndex = nodeExist(graph, secondNodeId);

	if (firstNodeIndex != -1 && secondNodeIndex != -1 && firstNodeId!=secondNodeId) {
		//first deal with connection between first->second
		//undirected graph so will repeat with second->first
		int addFirstEdge = addEdgeHelper(graph, firstNodeIndex, secondNodeId);
		int addSecondEdge = addEdgeHelper(graph, secondNodeIndex, firstNodeId);	

		if (addFirstEdge!=204 && addSecondEdge!=204) {
			return 200;
		}

		else {
			return 204;
		}
	}

	else {
		return 400;
	}
}

int remEdgeHelper(struct Graph* graph, uint64_t nodeIndex, uint64_t connectedNodeId) {
	struct Edge* temp = graph->nodeArray[nodeIndex].head;
	struct Edge* prev = NULL;

	while(temp->connectedId != connectedNodeId && temp->next != NULL) {
		prev = temp;
		temp = temp->next;
	}

	if (temp->connectedId == connectedNodeId) {
		if (prev) {
			prev->next = temp->next;
		}

		else {
			graph->nodeArray[nodeIndex].head = temp->next;
		}

		free(temp);
	}

	graph->nodeArray[nodeIndex].neighborCount--;
	return 1;
}

int remEdge(struct Graph* graph, uint64_t firstNodeId, uint64_t secondNodeId) {
	int firstNodeIndex = nodeExist(graph, firstNodeId);
	int secondNodeIndex = nodeExist(graph, secondNodeId);

	printf("\nremoved connection between: %" PRIu64 " %" PRIu64 "\n", firstNodeId, secondNodeId);

	if (firstNodeIndex != -1 && secondNodeIndex != -1 && firstNodeId!=secondNodeId) {
		int doesEdgeExist = edgeExist(graph, firstNodeIndex, secondNodeId);

		if (doesEdgeExist == 1) {
			int one = remEdgeHelper(graph, firstNodeIndex, secondNodeId);
			int two = remEdgeHelper(graph, secondNodeIndex, firstNodeId);

			if (one == 1 && two == 1) {
				return 200;
			}

			else {
				return 204;
			}
		}

		else {
			return 400;
		}
	}

	else {
		return 400;
	}
}

uint64_t* getNeighbors(struct Graph* graph, uint64_t nodeIndex) {
	uint64_t neighborCount = graph->nodeArray[nodeIndex].neighborCount;
	uint64_t* neighborArray = (uint64_t*)malloc(neighborCount * sizeof(uint64_t));
	struct Edge* head = graph->nodeArray[nodeIndex].head;

	for (uint64_t i =0; i<neighborCount; i++) {
		neighborArray[i] = head->connectedId;
		head = head->next;
	}

	return neighborArray;
}

void printGraph(struct Graph* graph) {
	printf("\n\nPRINTING GRAPH:::::::\n");
	int graphSize = graph->size;
	for (int i=0; i<graphSize; i++) {
		int nodeId = graph->nodeArray[i].nodeId;
		uint64_t nodeIndex = nodeExist(graph, nodeId);
		printf("%i index: %" PRIu64 " connections: ", nodeId, nodeIndex);

		int neighborCount = graph->nodeArray[i].neighborCount;
		if (neighborCount != 0) {
			printf("(neighbors: %i) ", neighborCount);
			struct Edge* temp = graph->nodeArray[i].head;
			int j = 0;
			while(temp) {
				printf("%" PRIu64 "(%i) -> ", temp->connectedId, j);
				temp = temp->next;
				j++;
			}
		}

		printf("\n");
	}

	printf("\n\nFINISHED PRINTING:::::\n");
}
