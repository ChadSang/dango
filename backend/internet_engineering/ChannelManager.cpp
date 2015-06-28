#include "ChannelManager.h"

ChannelManager::ChannelManager(int channel_cnt, int radius) :
channel_cnt(channel_cnt), nodes(new NodeManager(channel_cnt, radius))
{
}

void ChannelManager::run()
{
	print();
	string command;
	while (getline(cin, command))
	{
		if (command == "q") return;
		flush();
		print();
		// Sleep(500);
	}
}

void ChannelManager::print()
{
	nodes->print();
	cout << "channel assignment: " << endl;
	// nodes->print_channels();
	connected_nodes = nodes->get_channels();
	for (int i = 0; i < channel_cnt; ++i)
	{
		cout << "\tchannel " << i;
		auto channel_users = connected_nodes[i];
		for (auto iter = channel_users.begin(); iter != channel_users.end(); ++iter)
		{
			cout << " (" << (*iter).first << "," << (*iter).second << ")";
		}
		cout << endl;
	}

	cout << "routing tables: " << endl;
	nodes->print_routing_tables();
}

void ChannelManager::flush()
{ // get node informations and refresh all channels;
	nodes->flush();
	auto communicable_nodes = nodes->get_communicable_nodes();
	int size = nodes->get_node_cnt();
	for (int i = 0; i < size; ++i)
	{
		for (auto it = communicable_nodes.lower_bound(i);
			it != communicable_nodes.upper_bound(i); ++it)
		{
			// cout << "communicable: " << i << " <=> " << (*it).second << endl;
			nodes->exchange_message(i, (*it).second);
		}
	}
}

NodeManager* ChannelManager::get_node_manager()
{
	return nodes;
}

void ChannelManager::output()
{
	nodes->output();
	cerr << "channel assignment: " << endl;
	// nodes->print_channels();
	connected_nodes = nodes->get_channels();
	for (int i = 0; i < channel_cnt; ++i)
	{
		cerr << "\tchannel " << i;
		auto channel_users = connected_nodes[i];
		for (auto iter = channel_users.begin(); iter != channel_users.end(); ++iter)
		{
			cerr << " (" << (*iter).first << "," << (*iter).second << ")";
		}
		cerr << endl;
	}

	cerr << "routing tables: " << endl;
	nodes->output_routing_tables();
}

Json::Value ChannelManager::to_json()
{
	Json::Value output;
	output["type"] = "channels";
	connected_nodes = nodes->get_channels();
	Json::Value channels(Json::arrayValue);
	for (int i = 0; i < channel_cnt; ++i)
	{
		auto channel_users = connected_nodes[i];
		Json::Value links(Json::arrayValue);
		for (auto iter = channel_users.begin(); iter != channel_users.end(); ++iter)
		{
			Json::Value link(Json::arrayValue);
			link.append(nodes->get_uuid_by_index((*iter).first));
			link.append(nodes->get_uuid_by_index((*iter).second));
			links.append(link);
		}
		channels.append(links);
	}
	output["channels"] = channels;
	return output;
}

ChannelManager::~ChannelManager()
{
}
