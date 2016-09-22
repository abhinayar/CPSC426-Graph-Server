#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include "graphFunctions.h"
#include "shortestPath.h"

struct ReturnObject {
    int httpStatus;
    int firstNodeId;
    int secondNodeId;
    bool inGraph;
    uint64_t* neighborArray;
    uint64_t neighborArrayLength;
    uint64_t distance;
};

struct ReturnObject* createRetObject() {
    struct ReturnObject* newRetObj = (struct ReturnObject*)malloc(sizeof(struct ReturnObject));
    newRetObj->httpStatus = -1;
    newRetObj->firstNodeId = -1;
    newRetObj->secondNodeId = -1;
    newRetObj->inGraph = false;
    newRetObj->neighborArray = NULL;
    newRetObj->distance = -1;

    return newRetObj;
}

struct ReturnObject* getFunction(struct Graph* graph, char request[]) {
    //First get the function name
    int requestLength = strlen(request);
    int substringLength = requestLength - 13;
    
    char substring[substringLength];
    strcpy(substring, request+13);

    int indexOfFirstWhitespace;
    char* pointerOfFirstWhitespace = strchr(substring, ' ');
    indexOfFirstWhitespace = (int)(pointerOfFirstWhitespace - substring);
    
    int lengthOfFunction = indexOfFirstWhitespace;
    char functionName[lengthOfFunction];

    strncpy(functionName, substring, lengthOfFunction * sizeof(char));
    functionName[lengthOfFunction] = '\0';

    //now get the json data
    //Get the number of inputs we are looking for
    const char* functionList[9];
    functionList[0] = "add_node";
    functionList[1] = "remove_node";
    functionList[2] = "get_node";
    functionList[3] = "get_neighbors";
    functionList[4] =  "add_edge";
    functionList[5] = "get_edge";
    functionList[6] = "remove_edge";
    functionList[7] = "shortest_path";
    functionList[8] = "print_graph";

    int numInputs = 0, functionIndex = -1;

    for (int i=0; i<9; i++) {
        if (strcmp(functionList[i], functionName) == 0) {
            functionIndex = i;
        }
    }

    if (functionIndex < 3) {
        numInputs = 1;
    }

    else {
        numInputs = 2;
    }

    //printf("\nfunction name: %s | functionIndex: %i\n", functionName, functionIndex);

    char jsonSubstring[1024];
    int indexOfFirstCurly;
    char* pointerToFirstCurly = strchr(substring, '{');
    indexOfFirstCurly = (int)(pointerToFirstCurly - substring);
    strcpy(jsonSubstring, substring+indexOfFirstCurly);

    //printf("\nThe JSON: %s\n", jsonSubstring);

    int firstNodeId = -1, secondNodeId = -1;

    struct ReturnObject* retObject = createRetObject();

    int i = 0;
    char* p = jsonSubstring;
    while (*p) { // While there are more characters to process...
        if (isdigit(*p)) { // Upon finding a digit, ...
            long val = strtol(p, &p, 10); // Read a number, ...

            if (i == 0) {
                retObject->firstNodeId = val;
            }

            else if (i == 1) {
                retObject->secondNodeId = val;
            }

            else {
                break;
            }

            i++;
        } 

        else { 
            p++;
        }
    }

    firstNodeId = retObject->firstNodeId, secondNodeId = retObject->secondNodeId;

    //now get into actually calling the functions
	if (functionIndex == 0) {
		retObject->httpStatus = addNode(graph, firstNodeId);
	}

	else if (functionIndex == 1) {
		retObject->httpStatus = remNode(graph, firstNodeId);
	}

	else if (functionIndex == 2) {
		retObject->httpStatus = getNode(graph, firstNodeId);
	}

	else if (functionIndex == 3) {
        int nodeIndex = nodeExist(graph, firstNodeId);

        retObject->neighborArrayLength = graph->nodeArray[nodeIndex].neighborCount;
        retObject->neighborArray = getNeighbors(graph, nodeIndex);
        retObject->httpStatus = -998;
	}
    
    else if (functionIndex == 4) {
    	retObject->httpStatus = addEdge(graph, firstNodeId, secondNodeId);
    }

    else if (functionIndex == 5) {
    	retObject->httpStatus = getEdge(graph, firstNodeId, secondNodeId);
    }

    else if (functionIndex == 6) {
    	retObject->httpStatus = remEdge(graph, firstNodeId, secondNodeId);
    }

    else if (functionIndex == 7) {
    	//shortest path function
        retObject->distance = shortestPath(graph, firstNodeId, secondNodeId);

        if (retObject->distance == -100) {
            retObject->httpStatus = 400;
        }

        else if (retObject->distance == 204) {
            retObject->httpStatus = 204;
        }

        else {
            retObject->httpStatus = -997;
        }
    }

    else if (functionIndex == 8) {
    	//print graph
    	printGraph(graph);
        retObject->httpStatus = 202;
    }

    printGraph(graph);
    return retObject;
}

static volatile int keepGoing = 1; //global var for termination
void sigintHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    //signal(SIGINT, sigintHandler);
    printf("Caught SIGINT, exiting now\n");
    keepGoing = 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_handler = &sigintHandler;
    sigfillset(&sa.sa_mask);

	int create_socket, new_socket;    
	socklen_t addrlen;    
	int bufsize = 5024;    
	char *buffer = malloc(bufsize);    
	struct sockaddr_in address;    

	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){    
	  //printf("The socket was created\n");
	}

	address.sin_family = AF_INET;    
	address.sin_addr.s_addr = INADDR_ANY;    
	address.sin_port = htons(atoi(argv[1]));    

	if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){    
	  //printf("Binding Socket\n");
	}

	struct Graph* graph = createGraph();

	while (keepGoing) {
        bzero(buffer, bufsize);
        if (listen(create_socket, 10) < 0) {    
         perror("server: listen");    
         exit(1);    
        }    

        if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {    
         perror("server: accept");    
         exit(1);    
        }    

        if (new_socket > 0){    
         //printf("The Client is connected...\n");
        }

        recv(new_socket, buffer, bufsize, 0);    
        printf("REQUEST:\n%s\n", buffer);   

        struct ReturnObject* retObject = getFunction(graph, buffer);
        int httpStatus = retObject->httpStatus, node1 = retObject->firstNodeId, node2 = retObject->secondNodeId;

        char reply[1024];

        if (httpStatus == 204 || httpStatus == 400 || httpStatus == -1) {
        	//if error statement, find other error values
            //printf("\nerror code\n");
        	snprintf(reply, 1024, "HTTP/1.1 %i OK\n\nBad Req. ID: %i", httpStatus, httpStatus);
        }

        else if (httpStatus == 202) {
            //printf("print graph");
            snprintf(reply, 1024, "HTTP/1.1 200 OK\nContent-Length: 17\nContent-Type: application/json\n\n{\"in_graph\":true}\r\n");
        }

        else if (httpStatus == 203) {
            //printf("print graph");
            snprintf(reply, 1024, "HTTP/1.1 400 OK\nContent-Length: 18\nContent-Type: application/json\n\n{\"in_graph\":false}\r\n");
        }

        else if (httpStatus == -998) {
            //deal with getNeighbors
            uint64_t* neighborArray = retObject->neighborArray;
            char neighborText[(retObject->neighborArrayLength*2)];

            int j = 0;
            for (int i=0; i < (retObject->neighborArrayLength*2); i++) {
                if (i%2 == 0) {
                    neighborText[i] = (int)neighborArray[j] + '0';
                    j++;
                }

                else {
                    neighborText[i] = ',';
                }
            }

            neighborText[retObject->neighborArrayLength*2 - 1] = '\0';
            snprintf(reply, 1024, "HTTP/1.1 200 OK\nContent-Length: %f\nContent-Type: application/json\n\n{\"node_id\":%i,\"neighbors\":[%s]}\r\n", 12 + (floor(log10(abs(node1))) + 1) + 14 + (retObject->neighborArrayLength*2), node1, neighborText);
        }

        else if (httpStatus == -997) {
            uint64_t distance = retObject->distance;
            printf("distance: %lld\n", distance);
        	snprintf(reply, 1024, "HTTP/1.1 200 OK\nContent-Length: %f\nContent-Type: application/json\n\n{\"node_a_id\":%i,\"node_b_id\":%i,\"distance\":%lld}\r\n", 39 + floor(log10(abs(node1)) + 1) + floor(log10(abs(node2)) + 1) + floor(log10(abs(distance)) + 1), node1, node2, distance);
        }

        else if (node1 != -1 && node2 == -1) {
            snprintf(reply, 1024, "HTTP/1.1 200 OK\nContent-Length: %f\nContent-Type: application/json\n\n{\"node_id\":%i}\r\n", 12 + floor(log10(abs(node1)) + 1) + floor(log10(abs(node2)) + 1), node1);
        }

        else if (node1 != -1 && node2 != -1) {
        	//if 2 nodes
            //printf("\nboth nodes\n");
            snprintf(reply, 1024, "HTTP/1.1 200 OK\nContent-Length: %f\nContent-Type: application/json\n\n{\"node_a_id\":%i,\"node_b_id\":%i}\r\n", 27 + floor(log10(abs(node1)) + 1) + floor(log10(abs(node2)) + 1), node1, node2);
        }
        //printf("PRINTING JSON: %s\n", reply);
        write(new_socket, reply, strlen(reply));
        free(retObject);
        close(new_socket);    
	}    

	close(create_socket); 
    exit(0);   
}
