#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "Node.h"

using namespace std;

class NodeManager
{
	vector<Node> all_nodes;
	vector<bool> valid_nodes;
	int valid_node_cnt;

	int channel_cnt;
	int radius_square; // maximum distance between two communicable nodes
	multimap<int, int> communicable_nodes;

public:
	NodeManager(int channel_cnt, int radius);

	void flush();
	void run();

	void print();
	void print_channels();
	void print_routing_tables();
	Json::Value routing_tables_to_json();

	vector<map<int, int> > get_channels();

	Node add_node(int x, int y, int vx, int vy);
	Node add_node(string uuid, int x, int y, int vx, int vy);

	bool update_node(string uuid, int vx, int vy);
	void del_node(int index);
	void del_node(Node n);
	bool del_node(string uuid);

	void set_location(Node n, int x, int y);
	void set_location(int index, int x, int y);
	void set_velocity(Node n, int x, int y);
	void set_velocity(int index, int x, int y);

	const multimap<int, int> & get_communicable_nodes();

	void exchange_message(int node_a, int node_b);
	void send_message(int source, int destination);

	string get_uuid_by_index(int index);
	int get_node_cnt();
	int get_valid_node_cnt();

	void output();
	void output_channels();
	void output_routing_tables();

	virtual ~NodeManager();
};

