#include <stdio.h>
#include <vector>
#include <algorithm>
#include "graph.h"
#include "mongoose.h"
using std::vector;

vector<vector <uint64_t> > graph;
static const char *s_http_port;
static struct mg_serve_http_opts s_http_server_opts;

static void handle_add_node_call(struct mg_connection *nc, struct http_message *hm)
{
	char buf[200];
  uint64_t result;
	uint64_t node_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_id");
	node_id = strtoul(p->ptr, NULL, 0);
	result = _add_node(graph, node_id);
	if(result)
	{
		sprintf(buf, "{\n \"node_id\": %lu \n}\n", node_id);
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
				"Content-Type: application/json\r\n\r\n%s",
				(int)strlen(buf), buf);
	}
	else
	{
		mg_printf(nc, "HTTP/1.1 204 OK\r\n");
	}
}

static void handle_add_edge_call(struct mg_connection *nc, struct http_message *hm)
{
	char buf[200];
  uint64_t result;
	uint64_t node_a_id, node_b_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_a_id");
	node_a_id = strtoul(p->ptr, NULL, 0);
	p = find_json_token(tokens, "node_b_id");
	node_b_id = strtoul(p->ptr, NULL, 0);
	result = _add_edge(graph, node_a_id, node_b_id);
	if(result == -1)
	{
		mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n");
	}
	else if(result == 0)
	{
		mg_printf(nc, "HTTP/1.1 204 OK\r\n");
	}
	else if(result == 1)
	{
		sprintf(buf, "{\n \"node_a_id\": %lu,\n \"node_b_id\": %lu\n}\n", node_a_id, node_b_id);
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
				"Content-Type: application/json\r\n\r\n%s",
				(int)strlen(buf), buf);
	}
}

static void handle_get_node_call(struct mg_connection *nc, struct http_message *hm)
{
	char buf[200];
  uint64_t result;
	uint64_t node_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_id");
	node_id = strtoul(p->ptr, NULL, 0);
	result = _get_node(graph, node_id);
	if(result != -1)
	{
		sprintf(buf, "{\n \"in_graph\": %lu \n}\n", 1);
	}
	else
	{
		sprintf(buf, "{\n \"in_graph\": %lu \n}\n", 0);
	}
	mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
			"Content-Type: application/json\r\n\r\n%s",
			(int)strlen(buf), buf);
}
static void handle_get_edge_call(struct mg_connection *nc, struct http_message *hm)
{
	char buf[200];
  uint64_t result;
	uint64_t node_a_id, node_b_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_a_id");
	node_a_id = strtoul(p->ptr, NULL, 0);
	p = find_json_token(tokens, "node_b_id");
	node_b_id = strtoul(p->ptr, NULL, 0);
	result = _get_edge(graph, node_a_id, node_b_id);
	if(result == -1)
	{
		mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n");
	}
	else 
	{
		sprintf(buf, "{\n \"in_graph\": %lu \n}\n", result);
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
				"Content-Type: application/json\r\n\r\n%s",
				(int)strlen(buf), buf);
	}
}
static void handle_get_neighbors_call(struct mg_connection *nc, struct http_message *hm)
{
	char buf[200];
	char *head = buf;
  uint64_t idx;
	uint64_t node_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_id");
	node_id = strtoul(p->ptr, NULL, 0);
	idx = _get_nbrs(graph, node_id);
	if(idx != -1 && graph[idx].size() > 1)
	{
		sprintf(head, "{\n \"node_id\": %lu,\n \"neighbors\": [", node_id);
		head = buf + strlen(buf);
		for(uint64_t i = 1; i < graph[idx].size(); i++)
		{
			if(i != (graph[idx].size() - 1)) 
			{
				sprintf(head,"%lu, ", graph[idx][i]);
			}
			else
			{
				sprintf(head,"%lu]\n}\n", graph[idx][i]);
			}
			head = buf + strlen(buf);
		}
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
				"Content-Type: application/json\r\n\r\n%s",
				(int)strlen(buf), buf);
  }
	else
	{
		mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n");
	}
}
static void handle_remove_node_call(struct mg_connection *nc, struct http_message *hm)
{
	char buf[200];
  uint64_t result;
	uint64_t node_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_id");
	node_id = strtoul(p->ptr, NULL, 0);
	result = _remove_node(graph, node_id);
	if(result)
	{
		sprintf(buf, "{\n \"node_id\": %lu \n}\n", node_id);
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
				"Content-Type: application/json\r\n\r\n%s",
				(int)strlen(buf), buf);
	}
	else
	{
		mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n");
	}
}
static void handle_remove_edge_call(struct mg_connection *nc, struct http_message *hm)
{
	char buf[200];
  uint64_t result;
	uint64_t node_a_id, node_b_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_a_id");
	node_a_id = strtoul(p->ptr, NULL, 0);
	p = find_json_token(tokens, "node_b_id");
	node_b_id = strtoul(p->ptr, NULL, 0);
	result = _remove_edge(graph, node_a_id, node_b_id);
	if(result == 0)
	{
		mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n");
	}
	else if(result == 1)
	{
		sprintf(buf, "{\n \"node_a_id\": %lu,\n \"node_b_id\": %lu\n}\n", node_a_id, node_b_id);
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
				"Content-Type: application/json\r\n\r\n%s",
				(int)strlen(buf), buf);
	}
}
static void handle_shortest_path_call(struct mg_connection *nc, struct http_message *hm)
{
	char n[100];
	char buf[200];
  uint64_t result;
	uint64_t node_a_id, node_b_id;
	struct json_token tokens[200];
	struct json_token *p;
	parse_json(hm->body.p, hm->body.len, tokens, sizeof(tokens)/sizeof(tokens[0]));
	p = find_json_token(tokens, "node_a_id");
	node_a_id = strtoul(p->ptr, NULL, 0);
	p = find_json_token(tokens, "node_b_id");
	node_b_id = strtoul(p->ptr, NULL, 0);
	result = _shortest_path(graph, node_a_id, node_b_id);
	if(result > 0)
	{
		sprintf(buf, "{\n \"distance\": %lu \n}\n", result);
		mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
				"Content-Type: application/json\r\n\r\n%s",
				(int)strlen(buf), buf);
	}
	else if(result == 0)
	{
		mg_printf(nc, "HTTP/1.1 204 OK\r\n");
	}
	else
	{
		mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n");
	}
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;

	switch (ev) {
		case MG_EV_HTTP_REQUEST:
			if (mg_vcmp(&hm->uri, "/api/v1/add_node") == 0) {
				handle_add_node_call(nc, hm);
			} else if (mg_vcmp(&hm->uri, "/api/v1/add_edge") == 0) {
				handle_add_edge_call(nc, hm);
			} else if (mg_vcmp(&hm->uri, "/api/v1/remove_node") == 0) {
				handle_remove_node_call(nc, hm);
			} else if (mg_vcmp(&hm->uri, "/api/v1/remove_edge") == 0) {
				handle_remove_edge_call(nc, hm);
			} else if (mg_vcmp(&hm->uri, "/api/v1/get_node") == 0) {
				handle_get_node_call(nc, hm);
			} else if (mg_vcmp(&hm->uri, "/api/v1/get_edge") == 0) {
				handle_get_edge_call(nc, hm);
			} else if (mg_vcmp(&hm->uri, "/api/v1/get_neighbors") == 0) {
				handle_get_neighbors_call(nc, hm);
			} else if (mg_vcmp(&hm->uri, "/api/v1/shortest_path") == 0) {
				handle_shortest_path_call(nc, hm);
			} else {
				mg_serve_http(nc, hm, s_http_server_opts);  /* Serve static content */
			}
			nc->flags |= MG_F_SEND_AND_CLOSE;
			break;
		default:
			break;
	}
}

int main(int argc, char *argv[]) {
	struct mg_mgr mgr;
	struct mg_connection *nc;
	int i;
	char *cp;
	char port[100];
#ifdef MG_ENABLE_SSL
	const char *ssl_cert = NULL;
#endif

	mg_mgr_init(&mgr, NULL);

	/* Process command line options to customize HTTP server */
	if(argc == 1)
	{
		fprintf(stderr, "Error! Please assign a specific port!\n");
		exit(1);
	}
	else
	{
		strcpy(port, argv[1]);
		if(atoi(port) >= 0 && atoi(port) <= 65535) {
			s_http_port = port;
	  } else {
			fprintf(stderr, "Error! Illegal port number! (Legal: 0~65525)\n");
			exit(0);
		}
	}

	/* Set HTTP server options */
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		fprintf(stderr, "Error starting server on port %s\n", s_http_port);
		exit(1);
	}

#ifdef MG_ENABLE_SSL
	if (ssl_cert != NULL) {
		const char *err_str = mg_set_ssl(nc, ssl_cert, NULL);
		if (err_str != NULL) {
			fprintf(stderr, "Error loading SSL cert: %s\n", err_str);
			exit(1);
		}
	}
#endif

	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";
	s_http_server_opts.enable_directory_listing = "yes";

	/* Use current binary directory as document root */
	if (argc > 0 && ((cp = strrchr(argv[0], '/')) != NULL ||
				(cp = strrchr(argv[0], '/')) != NULL)) {
		*cp = '\0';
		s_http_server_opts.document_root = argv[0];
	}

	printf("Starting RESTful server on port %s\n", s_http_port);
	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return 0;
}
