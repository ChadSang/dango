#include "NodeManager.h"

	NodeManager::NodeManager(int channel_cnt, int radius) :
		channel_cnt(channel_cnt), radius_square(radius * radius)
		{
		// add_node(0, 0, 0, 0);
		// add_node(0, 0, 1, 0);
		// add_node(0, 0, 4, 0);
		// add_node(0, 0, 5, 0);
		// add_node(9, 0, 5, 0);
		// add_node(18, 0, 5, 0);
	}
	void NodeManager::flush()
	{
	// refresh all nodes and recalculate the communicable nodes
		communicable_nodes.clear();
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i])
			{
				all_nodes[i].move();
			}
			for (int j = 0; j < i; ++j)
			{
				if (valid_nodes[j] &&
					all_nodes[i].distance_square(all_nodes[j]) < radius_square)
					{
					communicable_nodes.insert(pair<int, int>(j, i));
				}
			}
		}

		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i])
			{
				all_nodes[i].check_channels(communicable_nodes);
			}
		}
	}
	void NodeManager::run()
	{
		print();
		string command;
		while (getline(cin, command))
		{
			if (command == "q") return;
			flush();
			print();
		}
	}
	void NodeManager::print()
	{
		cout << valid_node_cnt << " nodes in total: " << endl;
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i]) all_nodes[i].print();
		}
		cout << communicable_nodes.size() << " lines in total: " << endl;
		for (auto it = communicable_nodes.begin(); it != communicable_nodes.end(); ++it)
		{
			cout << (*it).first << " <=> " << (*it).second << endl;
		}
	}
	void NodeManager::print_channels()	{
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i]) all_nodes[i].print_channels();
		}
	}
	void NodeManager::print_routing_tables()
	{
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i]) all_nodes[i].print_routing_table();
		}
	}
	Json::Value NodeManager::routing_tables_to_json()
	{
		Json::Value output;
		output["type"] = "routes";

		Json::Value nodes(Json::arrayValue);
		int node_size = all_nodes.size();
		for (int node_index = 0; node_index < node_size; ++node_index)
		{
			if (valid_nodes[node_index])
			{
				Json::Value node;
				node["uuid"] = get_uuid_by_index(node_index);

				Json::Value routes = all_nodes[node_index].routing_table_to_json();
				int route_size = routes.size();
				for (int route_index = 0; route_index < route_size; ++route_index)
				{
					routes[route_index]["dest"] =
						get_uuid_by_index(routes[route_index]["dest"].asInt());
					routes[route_index]["via"] =
						get_uuid_by_index(routes[route_index]["via"].asInt());
				}
				
				node["routes"] = routes;
				nodes.append(node);
			}
		}

		output["nodes"] = nodes;
		return output;
	}
	vector<map<int, int> > NodeManager::get_channels()
	{
		vector<map<int, int> > connected_nodes(channel_cnt);
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i]) all_nodes[i].add_channels_to_result(connected_nodes);
		}
		return connected_nodes;
	}
	Node NodeManager::add_node(int x, int y, int vx, int vy)
	{
		Node n(all_nodes.size(), x, y, vx, vy, channel_cnt);
		all_nodes.push_back(n);
		valid_nodes.push_back(true);
		++valid_node_cnt;
		return n;
	}
	Node NodeManager::add_node(string uuid, int x, int y, int vx, int vy)
	{
		Node n(all_nodes.size(), uuid, x, y, vx, vy, channel_cnt);
		all_nodes.push_back(n);
		valid_nodes.push_back(true);
		++valid_node_cnt;
		return n;
	}
	bool NodeManager::update_node(string uuid, int vx, int vy)
	{
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (all_nodes[i].get_uuid() == uuid)
			{
				if (!valid_nodes[i])				{
					++valid_node_cnt;
					valid_nodes[i] = true;
				}
				all_nodes[i].set_speed(vx, vy);
			}
		}
		
		// uuid not found
		return false;
	}
	void NodeManager::del_node(int index)
	{
		if (valid_nodes[index])
		{
			valid_nodes[index] = false;
			--valid_node_cnt;
		}
	}
	void NodeManager::del_node(Node n)
	{
		del_node(n.get_index());
	}
	bool NodeManager::del_node(string uuid)
	{
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (all_nodes[i].get_uuid() == uuid)
			{
				del_node(i);
				return true;
			}
		}

		// uuid not found
		return false;
	}

	void NodeManager::set_location(Node n, int x, int y)
	{
		all_nodes[n.get_index()].set_location(x, y);
	}
	void NodeManager::set_location(int index, int x, int y)
	{
		all_nodes[index].set_location(x, y);
	}

	void NodeManager::set_velocity(Node n, int x, int y)
	{
		all_nodes[n.get_index()].set_speed(x, y);
	}
	void NodeManager::set_velocity(int index, int x, int y)
	{
		all_nodes[index].set_speed(x, y);
	}

	const multimap<int, int> & NodeManager::get_communicable_nodes()
	{ // called by ChannelManager
		return communicable_nodes;
	}
	void NodeManager::exchange_message(int node_a, int node_b)
	{
		send_message(node_a, node_b);
		send_message(node_b, node_a);
	}
	void NodeManager::send_message(int source, int destination)
	{
		all_nodes[destination].receive(all_nodes[source]);
	}
	string NodeManager::get_uuid_by_index(int index)
	{
		assert(index >= 0);
		return all_nodes[index].get_uuid();
	}
	int NodeManager::get_node_cnt()
	{
		return all_nodes.size();
	}
	int NodeManager::get_valid_node_cnt()
	{
		return valid_node_cnt;
	}
	void NodeManager::output()
	{
		cerr << valid_node_cnt << " nodes in total: " << endl;
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i]) all_nodes[i].output();
		}
		cerr << communicable_nodes.size() << " lines in total: " << endl;
		for (auto it = communicable_nodes.begin(); it != communicable_nodes.end(); ++it)
		{
			cerr << (*it).first << " <=> " << (*it).second << endl;
		}
	}
	void NodeManager::output_channels()
	{
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i]) all_nodes[i].output_channels();
		}
	}
	void NodeManager::output_routing_tables()
	{
		int size = all_nodes.size();
		for (int i = 0; i < size; ++i)
		{
			if (valid_nodes[i]) all_nodes[i].output_routing_table();
		}
	}

NodeManager::~NodeManager()
{
}
