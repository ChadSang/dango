#pragma once

#include <vector>
#include <map>

#include "NodeManager.h"

using namespace std;

class ChannelManager
{
	int channel_cnt; // number of available channels for all nodes
	NodeManager* nodes;
	vector<map<int, int> > connected_nodes;

public:
	ChannelManager(int channel_cnt, int radius);
	void run();
	void print();
	void flush();
	NodeManager* get_node_manager();
	void output();

	Json::Value to_json();
	virtual ~ChannelManager();
};

