#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cassert>

#include "json\json.h"
#include "json\json-forwards.h"

using namespace std;

class NodeManager;

class Node
{
	int index;
	string uuid;

	// location and velocity
	int x;
	int y;
	int vx;
	int vy;

	int channel_cnt;
	vector<int> channel_user; // current user's ID(node) of each channel, -1 for unknown
	vector<bool> my_channel; // if the channel is used by this node

	map<int, pair<int, int> > routing_table; // dst(node_id) => pair(channel_id, distance)

public:
	Node(int index, int channel_cnt) : index(index), x(0), y(0), vx(0), vy(0),
		channel_cnt(channel_cnt), channel_user(channel_cnt, -1), my_channel(channel_cnt, false) {
	}
	Node(int index, int x, int y, int vx, int vy, int channel_cnt) :
		index(index), x(x), y(y), vx(vx), vy(vy),
		channel_cnt(channel_cnt), channel_user(channel_cnt, -1), my_channel(channel_cnt, false) {
	}
	Node(int index, string uuid, int x, int y, int vx, int vy, int channel_cnt) :
		index(index), uuid(uuid), x(x), y(y), vx(vx), vy(vy),
		channel_cnt(channel_cnt), channel_user(channel_cnt, -1), my_channel(channel_cnt, false) {
	}
	void check_channels(const multimap<int, int> &communicable_nodes) {
		for (int i = 0; i < channel_cnt; ++i) {
			int user = channel_user[i];
			if (user != -1) {
				int min = user < index ? user : index;
				int max = user > index ? user : index;

				// multimap<int, int>::iterator iter;
				// pair<multimap<int, int>::iterator, multimap<int, int>::iterator> ret;
				bool connected = false;
				auto ret = communicable_nodes.equal_range(min);
				for (auto iter = ret.first; iter != ret.second; ++iter) {
					if ((*iter).second == max) {
						connected = true;
						break;
					}
				}
				if (!connected) {
					channel_user[i] = -1;
					my_channel[i] = false;
				}
			}
		}
		check_routing_table();
	}
	void check_routing_table() {
		for (auto iter = routing_table.begin(); iter != routing_table.end();) {
			if (!my_channel[iter->second.first]) {
				// this node can't use the channel in the routing_table
				routing_table.erase(iter++);
			}
			else {
				// TODO: what if in one cycle,
				// this node release the channel and then connect to another node by this channel
				++iter;
			}
		}
	}
	const map<int, pair<int, int> > get_routing_table() {
		return routing_table;
	}
	void receive(Node &n) {
		if (channel_handler(n)) {
			// two nodes are still connected
			route_handler(n);
		}
	}
	bool channel_handler(Node &n) {
		// cout << index << " get message from " << n.index << endl;
		// cout << "BEFORE" << endl;
		// print();
		// n.print();
		// cout << "AFTER" << endl;
		bool connected = false;
		for (int i = 0; i < channel_cnt; ++i) {
			if (channel_user[i] == n.index) {
				if (my_channel[i]) {
					if (!n.is_used_by_this_node(i)) {
						// src node closes the connection
						// cout << " src node closes the connection " << endl;
						channel_user[i] = -1;
						my_channel[i] = false;
					}
					else {
						// src node and dst node have been connected
						// cout << " src node and dst node have been connected " << endl;
						connected = true;
					}
				}
				else if (!my_channel[i] && !(n.is_used_by_this_node(i))) {
					channel_user[i] = -1;
				}
			}
		}
		for (int i = 0; i < channel_cnt; ++i) {
			if (n.is_used_by_this_node(i)) {
				// the channel i is used by src node 
				// cout << " the channel " << i << " is used by src node " << endl;
				if (channel_user[i] != -1
					&& channel_user[i] != n.index
					&& channel_user[i] != n.get_user_by_channel(i)) {
					// conflict: tell src node to release this channel
					// cout << " conflict: tell src node to release this channel " << endl;
					n.set_used_by_this_node(i, false);
				}
				else if (channel_user[i] == -1) {
					channel_user[i] = n.index;
					my_channel[i] = false;
				}
			}
		}
		if (!connected) {
			// try to find an available channel 
			// cout << " try to find an available channel " << endl;
			for (int i = 0; i < channel_cnt; ++i) {
				if (n.get_user_by_channel(i) == -1
					&& channel_user[i] == -1) {
					channel_user[i] = n.index;
					my_channel[i] = true;
					n.set_user_by_channel(i, index);
					n.set_used_by_this_node(i, true);
					connected = true;
					break;
				}
			}
		}
		// print();
		// n.print();
		return connected;
	}
	void route_handler(Node n) {// dst(node_id) => pair(channel_id, distance)
		// cout << "BEGIN" << endl;
		int channel_id = -1;
		for (int i = 0; i < channel_cnt; ++i) {
			if (channel_user[i] == n.index && my_channel[i] == true) {
				channel_id = i;
				break;
			}
		}
		assert(channel_id != -1);
		routing_table[n.index] = pair<int, int>(channel_id, 1);
		// cout << index << "\t" << routing_table.size() << endl;
		
		for (auto iter = routing_table.begin(); iter != routing_table.end();) {
		// cout << index << "\t" << routing_table.size() << endl;
			if (iter->second.first == channel_id) {
				int dist = n.get_logical_distance(index, iter->first);
				if (dist == -1) {
					// damaged route
					routing_table.erase(iter++);
				}
				else {
					iter++->second.second = dist + 1;
				}
			}
			else {
				++iter;
			}
		}
		for (auto iter : n.get_routing_table()) {
		// cout << index << "\t" << routing_table.size() << endl;
			if (iter.first == index) continue;
			if (n.get_logical_distance(index, iter.first) == -1) continue;

			if (!routing_table.count(iter.first)
				|| routing_table[iter.first].second > iter.second.second + 1) {
				iter.second.first = channel_id;
				++iter.second.second; // ++distance
				routing_table.insert(iter);
			}
		}
		// cout << "END" << endl;
	}
	int get_user_by_channel(int channel_id) {
		return channel_user[channel_id];
	}
	void set_user_by_channel(int channel_id, int user_id) {
		channel_user[channel_id] = user_id;
	}
	bool is_used_by_this_node(int channel_id) {
		return my_channel[channel_id];
	}
	void set_used_by_this_node(int channel_id, bool flag) {
		my_channel[channel_id] = flag;
	}

	void set_speed(int vx, int vy) {
		Node::vx = vx;
		Node::vy = vy;
	}
	void move() {
		x += vx;
		y += vy;
	}
	int distance_square(Node n) {
		int dx = n.x - x;
		int dy = n.y - y;
		return dx * dx + dy * dy;
	}
	int get_logical_distance(int src, int node_id) {
		if (node_id == index) {
			return 0;
		}
		if (routing_table.count(node_id)) {
			if (src == channel_user[routing_table[node_id].first]) {
				return -1;
			}
			return routing_table[node_id].second;
		}
		return -1;
	}
	void print() {
		cout << "Node " << index << ": (" << x << ',' << y << ") v=(" << vx << ',' << vy << ")" << endl;
		for (int i = 0; i < channel_cnt; ++i) {
			cout << '\t' << channel_user[i] << '(' << my_channel[i] << ')';
		}
		cout << endl;
	}
	void print_channels() {
		for (int i = 0; i < channel_cnt; ++i) {
			if (my_channel[i]) {
				cout << index << " => " << channel_user[i] << " (by channel " << i << ")" << endl;
			}
		}
	}
	void print_routing_table() {
		cout << "node " << index << endl;
		for (auto iter : routing_table) {
			cout << "\tTo node " << iter.first <<
				"(by node " << channel_user[iter.second.first] <<
				",channel " << iter.second.first <<
				",distance " << iter.second.second << ")" << endl;
		}
	}
	Json::Value routing_table_to_json() {
		Json::Value routes;
		for (auto iter : routing_table) {
			Json::Value route;

			route["dest"] = iter.first;
			route["dist"] = iter.second.second;
			route["channel"] = iter.second.first;
			route["via"] = channel_user[iter.second.first];
			
			routes.append(route);
		}
		return routes;

	}
	void add_channels_to_result(vector<map<int, int> > &connected_nodes) {
		for (int i = 0; i < channel_cnt; ++i) {
			if (my_channel[i]) {
				int min = index < channel_user[i] ? index : channel_user[i];
				int max = index > channel_user[i] ? index : channel_user[i];
				if (connected_nodes[i].count(min) && connected_nodes[i][min] != max) {
					cout << "CHANNEL ASSIGNMENT FAILURE" << endl;
				}
				else {
					connected_nodes[i][min] = max;
				}
			}
		}
	}
	int get_index() {
		return index;
	}
	string get_uuid() {
		return uuid;
	}
	
	virtual ~Node();
};

