#include <stdio.h>
#include <vector>
#include <algorithm>
#include <stdint.h>
using std::vector;
uint64_t _get_node(vector<vector <uint64_t> >& graph, uint64_t node_id);
int _get_edge(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id);
uint64_t _get_nbrs(vector<vector <uint64_t> >& graph, uint64_t node_id);
int _add_node(vector<vector <uint64_t> >& graph, uint64_t node_id);
int _add_edge(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id);
int _remove_node(vector<vector <uint64_t> >& graph, uint64_t node_id);
int _remove_edge(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id);
uint64_t _shortest_path(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id);
