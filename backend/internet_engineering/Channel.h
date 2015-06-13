#pragma once

#include <vector>

using namespace std;

class Channel
{
	int index;
	vector<pair<int, int> > occupied_node_indices;
	
public:
	Channel();
	~Channel();
};

