#define __STDC_FORMAT_MACROS
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
#include "mongoose.h"

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

struct ReturnObject* getFunction(struct Graph* graph, char request[], int length) {
    //Oh boy another LONG convoluted function for string parsing in C

    //First let us null terminate the request string from Mongoose
    request[length] = '\0';
    //printf("req: %s\n", request);

    //First get the function name
    //Get request length
    int requestLength = strlen(request);
    //Substring length is reqLength - 13 because the func. name starts at the 13th char
    int substringLength = requestLength - 13;
    
    //Create the buffer/string
    char substring[substringLength];
    //copy the request starting at the 13th char 
    strcpy(substring, request+13);
    //printf("substr: %s\n", substring);

    //The first whitespace is the first char after the funct name
    int indexOfFirstWhitespace;
    //Find the index of it by strchr, etc.
    char* pointerOfFirstWhitespace = strchr(substring, ' ');
    indexOfFirstWhitespace = (int)(pointerOfFirstWhitespace - substring);
    //The index of the first whitespace will be the length of the funct.
    int lengthOfFunction = indexOfFirstWhitespace;
    //define functionName buffer
    char functionName[lengthOfFunction];
    //strncpy that shit...
    strncpy(functionName, substring, lengthOfFunction * sizeof(char));
    //null terminate
    functionName[lengthOfFunction] = '\0';
    printf("func name: %s\n", functionName);

    //now get the json data

    //function array to iterate over
    //because we cant do things like string to function conversion in C.
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

    //Some pretty useful vars.
    int functionIndex = -1;

    //compare the parsed function name to the array so we can figure out what it is that we want to do.
    for (int i=0; i<9; i++) {
        if (strcmp(functionList[i], functionName) == 0) {
            functionIndex = i;
        }
    }

    //now let's get the JSON w some string bs (or magic)
    //MAX for json has got to be something like 1024
    //But we will adjust as needed.

    //First curly indicates start of JSON
    int indexOfFirstCurly;
    //find the ptr to 1st {
    char* pointerToFirstCurly = strchr(substring, '{');
    //find the index
    indexOfFirstCurly = (int)(pointerToFirstCurly - substring);
    //length of JSON will be length of sbstring - index of first curly (no payload after jSON assumption... please don't be wrong)
    //Honestly making a bunch of assumptions about input validity here so pray for me. (i.e. the specs could be more detailed)
    int lengthOfJSON = (int)strlen(substring) - (int)indexOfFirstCurly;
    //printf("length of json: %i", lengthOfJSON+1);

    //define json buff
    char jsonSubstring[lengthOfJSON];
    //use STRNcpy... hear its safer than strcpy
    strncpy(jsonSubstring, &substring[indexOfFirstCurly], lengthOfJSON * sizeof(char));
    //null term.
    jsonSubstring[lengthOfJSON+1] = '\0';

    //printf("\nThe JSON: %s\n", jsonSubstring);

    //create the RETURN OBJECT
    //We're finally going to do something returnable
    struct ReturnObject* retObject = createRetObject();

    //PARSE THE JSON TO GET THE INT VALS
    //Hacked together after a lot of Stackoverflowing and soul searching
    //Its 3AM give my humor a break.
    int i = 0;
    char* p = jsonSubstring;
    while (*p) { // While there are more characters to process...
        if (isdigit(*p)) { // Upon finding a digit, ...
            long val = strtol(p, &p, 10); // Read a number, ...

            //ASSUMING THERE ARE ONLY TWO ID'S PASSED MAX
            //WILL break for more than two
            // slash will only get the first 2 nums.

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

    //some vars for the hell of it.
    int firstNodeId = -1, secondNodeId = -1;
    //Set the values of those wonderful vars we created for ease of use.
    firstNodeId = retObject->firstNodeId, secondNodeId = retObject->secondNodeId;

    //now get into actually calling the functions
    //Mess of case function type things
    //BUT my C skills suck and idk how to write those so YOLO.
    //function values correspond to functionList array... don't be lazy you can look.
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

    //why not print the graph anyways?
    //Cant fail the pset worse than I already am right?
    //printGraph(graph);
    return retObject;
}

char* parse(struct ReturnObject* retObject, char* reply) {
    //Begin the long convoluted parsing function

    //Get the HTTPStatus from retObject
    //httpStatus may not always reflect the actual HTTP Status
    //Sometimes it returns codes which we will parse/analyze here into a char buffer
    int httpStatus = retObject->httpStatus, node1 = retObject->firstNodeId, node2 = retObject->secondNodeId;

    //These are the error httpStatusie
    if (httpStatus == 204 || httpStatus == 400 || httpStatus == -1) {
        if (httpStatus == 204) {
            snprintf(reply, 1024, "HTTP/1.1 %i OK\r\nContent-Length: 0\r\n\r\n", httpStatus);
        }

        else {
            snprintf(reply, 1024, "HTTP/1.1 %i Bad Request\r\nContent-Length: 0\r\n\r\n", httpStatus);
        }
    }

    //This is the in_graph for get_node 
    else if (httpStatus == 202) {
        //printf("print graph");
        snprintf(reply, 1024, "HTTP/1.1 200 OK\r\nContent-Length: 17\r\nContent-Type: application/json\r\n\r\n{\"in_graph\":true}\r\n");
    }

    //This is the in_graph for get_edge
    else if (httpStatus == 203) {
        //printf("print graph");
        snprintf(reply, 1024, "HTTP/1.1 400 OK\r\nContent-Length: 18\r\nContent-Type: application/json\r\n\r\n{\"in_graph\":false}\r\n");
    }

    //This is the getNeighbors return code
    else if (httpStatus == -998) {
        //deal with getNeighbors
        uint64_t* neighborArray = retObject->neighborArray;

        int j = 0;
        char textArray[(retObject->neighborArrayLength*2)-1];
        for (int i=0; i<(retObject->neighborArrayLength*2)-1; i++) {
            if (i % 2 == 0) {
                j += snprintf(&textArray[i], (retObject->neighborArrayLength*2)-1, "%i", (int)neighborArray[j]);
            }

            else {
                sprintf(&textArray[i], ",");
            }
        } 

        
        printf("print neighbor array [ ");
        for (int i=0; i < (retObject->neighborArrayLength*2-1); i++) {
            printf("%c ", textArray[i]);
        }
        printf("]\n");

        snprintf(reply, 1024, "HTTP/1.1 200 OK\nContent-Length: %i\nContent-Type: application/json\n\n{\"node_id\":%i,\"neighbors\":[%s]}\r\n", 12 + (int)(floor(log10(abs(node1))) + 1) + 16 + (int)(retObject->neighborArrayLength*2), node1, textArray);
    }

    //This is the shortest path ret code
    else if (httpStatus == -997) {
        uint64_t distance = retObject->distance;
        //printf("distance: %" PRIu64 "\n", distance);
        snprintf(reply, 1024, "HTTP/1.1 200 OK\r\nContent-Length: %i\r\nContent-Type: application/json\r\n\r\n{\"distance\":%i}\r\n", 13 + (int)floor(log10(abs(distance)) + 1), (int)distance);
    }

    //basically at ALL other times when we have the first node and not the other
    else if (node1 != -1 && node2 == -1) {
        snprintf(reply, 1024, "HTTP/1.1 200 OK\r\nContent-Length: %i\r\nContent-Type: application/json\r\n\r\n{\"node_id\":%i}\r\n", 12 + (int)floor(log10(abs(node1))) + 1, node1);
    }

    //all other times we have both codes
    else if (node1 != -1 && node2 != -1) {
        //if 2 nodes
        //printf("\nboth nodes\n");
        snprintf(reply, 1024, "HTTP/1.1 200 OK\r\nContent-Length: %i\r\nContent-Type: application/json\r\n\r\n{\"node_a_id\":%i,\"node_b_id\":%i}\r\n", 27 + (int)floor(log10(abs(node1)) + 1) + (int)floor(log10(abs(node2))+1), node1, node2);
    }

    return reply;
}

struct Graph* graph;

// Define an event handler function
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct mbuf *io = &nc->recv_mbuf;
  struct ReturnObject* retObject;

  int buffSize = 1024;
  char* buff = malloc(sizeof(char) * buffSize);

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      // This event handler implements simple TCP echo server
      retObject = getFunction(graph, io->buf, io->len);

      buff = parse(retObject, buff);
      printf("\nbuff: %s", buff);
      mg_send(nc, buff, strlen(buff));
      //mg_send(nc, io->buf, io->len);  // Echo received data back
      mbuf_remove(io, io->len);      // Discard data from recv buffer
      break;
    default:
      break;
  }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        perror("Please enter the port #");
        exit(255);
    }

    graph = createGraph();
    struct mg_mgr mgr;
    struct mg_connection *nc;
    mg_mgr_init(&mgr, NULL);  // Initialize event manager object

    // Note that many connections can be added to a single event manager
    // Connections can be created at any point, e.g. in event handler function
    nc = mg_bind(&mgr, argv[1], ev_handler);  // Create listening connection and add it to the event manager

    // listen for http
    mg_set_protocol_http_websocket(nc);

    for (;;) {  // Start infinite event loop
    mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    return 0;
}
