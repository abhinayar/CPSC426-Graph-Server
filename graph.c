#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include "graph.h"

// Get node: 
// return the location of node with node_id, return -1 if not exist.
uint64_t _get_node(vector<vector <uint64_t> >& graph, uint64_t node_id)
{
	for(uint64_t i = 0; i < graph.size(); i++)
	{
		if(node_id == graph[i][0])
			return i;
	}
	return -1;
}

// Get edge:
// return -1 if one of the vertices does not exist, 0 if the edge doesn't exist, 1 if the edge exists.
int _get_edge(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id)
{
	uint64_t idx_a, idx_b;
	if (node_a_id == node_b_id)
	{
		return 0;
	}
	else if((idx_a = _get_node(graph, node_a_id)) == -1 || (idx_b = _get_node(graph, node_b_id)) == -1) // Check one of the vertices is in the graph or not
	{
		return -1;
	}
	else
	{
		for(uint64_t i = 1; i < graph[idx_a].size(); i++)
		{
			if(graph[idx_a][i] == node_b_id)
			{
				return 1;
			}
		}
	}
	return 0;
}

// Get neighbors:
// return	index of the node in the graph (i.e graph[idx]) if success, return -1 if the node does not exist.
uint64_t _get_nbrs(vector<vector <uint64_t> >& graph, uint64_t node_id)
{
	uint64_t idx;
	if((idx = _get_node(graph, node_id)) == -1)
	{
		return -1;
	}
	else 
	{
		return idx;
	}
}
// Add node:
// return 1 if success, 0 if the node already exists.
int _add_node(vector<vector <uint64_t> >& graph, uint64_t node_id)
{
	if(_get_node(graph, node_id) != -1)
	{
		return 0;
	}
	else 
	{
		vector <uint64_t> new_node;
		new_node.push_back(node_id);
		graph.push_back(new_node);
		return 1;
	}
}

// Add edge:
// return -1 if node does not exist or a = b, 0 if edge already exists, 1 if success
int _add_edge(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id)
{
	uint64_t idx_a, idx_b;
	if(node_a_id == node_b_id || (idx_a = _get_node(graph, node_a_id)) == -1 || (idx_b = _get_node(graph, node_b_id)) == -1)
	{
		return -1;
	}
	else if(_get_edge(graph, node_a_id, node_b_id) == 1)
	{
		return 0;
	}
	else
	{
		graph[idx_a].push_back(node_b_id);
		graph[idx_b].push_back(node_a_id);
		return 1;
	}
}

// Remove node:
// return 1 if success, 0 if the node does not exist.
int _remove_node(vector<vector <uint64_t> >& graph, uint64_t node_id)
{
	uint64_t idx;
	if((idx = _get_node(graph, node_id)) == -1)
	{
		return 0;
	}
	else
	{
		for(uint64_t i = 0; i < graph[idx].size(); i++)
		{
			_remove_edge(graph, node_id, graph[idx][i]);
		}
		graph.erase(graph.begin()+idx);
		return 1;
	}
}

// Remove edge:
// return 1 if success, 0 if the edge does not exist.
int _remove_edge(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id)
{
	uint64_t idx_a, idx_b;
	if(_get_edge(graph, node_a_id, node_b_id) <= 0)
	{
		return 0;
	}
	else
	{
		idx_a = _get_node(graph, node_a_id);
		idx_b = _get_node(graph, node_b_id);
		//printf("[_remove_edge] node_a: %d, node_b: %d, idx_a: %d, idx_b: %d, a_size: %d, b_size: %d\n", node_a_id, node_b_id, idx_a, idx_b, graph[idx_a].size(), graph[idx_b].size());
		for(uint64_t i = 1; i < graph[idx_a].size(); i++)
		{
			if(graph[idx_a][i] == node_b_id)
			{
				graph[idx_a].erase(graph[idx_a].begin()+i);
			}
		}
		for(uint64_t i = 1; i < graph[idx_b].size(); i++)
		{
			if(graph[idx_b][i] == node_a_id)
			{
				graph[idx_b].erase(graph[idx_b].begin()+i);
			}
		}
		return 1;
	}
}

// Shortest path:
// return distance if two nodes are connected, 0 if no path, -1 if either node does not exist
uint64_t _shortest_path(vector<vector <uint64_t> >& graph, uint64_t node_a_id, uint64_t node_b_id)
{
	uint64_t idx_a, idx_b;
	uint64_t idx_nbr;
	if(node_a_id == node_b_id)
	{
		return 0;
	}
	else if((idx_a = _get_node(graph, node_a_id)) == -1 || (idx_b = _get_node(graph, node_b_id)) == -1)
	{
		return -1;
	}
	else 
	{
		vector<vector <uint64_t> > distance(graph.size(), vector <uint64_t>(2));
		for(uint64_t i = 0; i < graph.size(); i++)
		{
			distance[i][0] = graph[i][0];
			distance[i][1] = -1;
		}
		distance[idx_a][1] = 0;
		for(uint64_t i = 1; i < graph[idx_a].size(); i++)
		{
			idx_nbr = _get_node(graph, graph[idx_a][i]);
			distance[idx_nbr][1] = 1;
		}
		int update = 1;
		int step = 1;
		while(update == 1)
		{
			update = 0;
			for(uint64_t i = 0; i < graph.size(); i++)
			{
				if(distance[i][1] == step)
				{
					//printf("for node %d, in the update stage, has %d nbrs\n", distance[i][0], graph[i].size());
					for(uint64_t j = 1; j < graph[i].size(); j++)
					{
						idx_nbr = _get_node(graph, graph[i][j]);
						//printf("node %d, has nbr %d with distance %d\n", distance[i][0], graph[idx_nbr][0], distance[idx_nbr][1]);
						if(distance[idx_nbr][1] == -1 || distance[idx_nbr][1] > step)
						{
							distance[idx_nbr][1] = step + 1;
							update = 1;
						}
					}
				}
				//printf("node_id %d, distance: %d\n", distance[i][0], distance[i][1]);
			}
			step = step + 1;
		}
		if(distance[idx_b][1] == -1)
		{
			return 0;
		}
		else
		{
			return distance[idx_b][1];
		}
	}
}
