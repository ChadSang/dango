#include "Node.h"

	Node::Node(int index, int channel_cnt) : index(index), x(0), y(0), vx(0), vy(0),
		sequence_number(0), broadcast_id(0),
		channel_cnt(channel_cnt), channel_user(channel_cnt, -1),
		my_channel(channel_cnt, false), master_user(channel_cnt, false)
	{
	}
	Node::Node(int index, int x, int y, int vx, int vy, int channel_cnt) :
		index(index), x(x), y(y), vx(vx), vy(vy),
		sequence_number(0), broadcast_id(0),
		channel_cnt(channel_cnt), channel_user(channel_cnt, -1),
		my_channel(channel_cnt, false), master_user(channel_cnt, false)
	{
	}
	Node::Node(int index, string uuid, int x, int y, int vx, int vy, int channel_cnt) :
		index(index), uuid(uuid), x(x), y(y), vx(vx), vy(vy),
		sequence_number(0), broadcast_id(0),
		channel_cnt(channel_cnt), channel_user(channel_cnt, -1),
		my_channel(channel_cnt, false), master_user(channel_cnt, false)
	{
	}

	void Node::add_capability(Physics::Capability capability)
	{
		assert(capability != Physics::Capability::NONE);
		capabilities.insert(capability);
	}

	bool Node::has_capability(Physics::Capability capability)
	{
		assert(capability != Physics::Capability::NONE);
		return capabilities.count(capability) > 0;
	}

	void Node::remove_capability(Physics::Capability capability)
	{
		assert(capability != Physics::Capability::NONE);
		capabilities.erase(capability);
	}

	void Node::check_channels(const multimap<int, int> &communicable_nodes)
	{
		for (int i = 0; i < channel_cnt; ++i)
		{
			int user = channel_user[i];
			if (user != -1)
			{
				// user is using channel i
				int min = user < index ? user : index;
				int max = user > index ? user : index;

				// multimap<int, int>::iterator iter;
				// pair<multimap<int, int>::iterator, multimap<int, int>::iterator> ret;
				bool in_range = false;

				auto ret = communicable_nodes.equal_range(min);
				for (auto iter = ret.first; iter != ret.second; ++iter)
				{
					if ((*iter).second == max)
					{
						in_range = true;
						break;
					}
				}

				if (!in_range)
				{
					channel_user[i] = -1;
					my_channel[i] = false;
				}
			}
			else {
				// nobody use this channel in the range
				my_channel[i] = false;
			}
		}
		check_routing_table();
	}
	void Node::check_routing_table()
	{
		for (auto iter = routing_table.begin(); iter != routing_table.end();)
		{
			if (!my_channel[iter->second.first])
			{
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
	const map<int, pair<int, int> > Node::get_routing_table()
	{
		return routing_table;
	}
	void Node::receive(Node &n)
	{
		if (channel_handler(n))
		{
			// two nodes are still connected
			route_handler(n);
		}
	}
	bool Node::channel_handler(Node &n)
	{
		// cout << index << " get message from " << n.index << endl;
		// cout << "BEFORE" << endl;
		// print();
		// n.print();
		// cout << "AFTER" << endl;
		bool connected = false;
		for (int i = 0; i < channel_cnt; ++i)
		{
			if (channel_user[i] == n.index)
			{
				// this node think that the channel i is used by n

				if (my_channel[i])
				{
					if (!n.is_used_by_this_node(i))
					{
						// src node closes the connection
						// cout << " src node closes the connection " << endl;

						// channel_user[i] = -1;
						my_channel[i] = false;
					}
					else
					{
						// src node and dst node have been connected
						// cout << " src node and dst node have been connected " << endl;
						connected = true;
					}
				}
				else if (!my_channel[i] && !(n.is_used_by_this_node(i)))
				{
					channel_user[i] = -1;
				}
			}
		}
		for (int i = 0; i < channel_cnt; ++i)
		{
			if (n.is_used_by_this_node(i))
			{
				// the channel i is used by src node 
				// cout << " the channel " << i << " is used by src node " << endl;
				if (channel_user[i] != -1
					&& channel_user[i] != n.index
					&& channel_user[i] != n.get_user_by_channel(i))
					{
					// conflict: tell src node to release this channel
					// cout << " conflict: tell src node to release this channel " << endl;
					n.set_used_by_this_node(i, false);
				}
				else if (channel_user[i] == -1)
				{
					channel_user[i] = n.index;
					my_channel[i] = false;
				}
			}
		}
		if (!connected)
		{
			// try to find an available channel 
			// cout << " try to find an available channel " << endl;
			for (int i = 0; i < channel_cnt; ++i)
			{
				if (n.get_user_by_channel(i) == -1
					&& channel_user[i] == -1)
					{
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
	void Node::route_handler(Node n)
	{// dst(node_id) => pair(channel_id, distance)
		// cout << "BEGIN" << endl;
		int channel_id = -1;
		for (int i = 0; i < channel_cnt; ++i)
		{
			if (channel_user[i] == n.index && my_channel[i] == true)
			{
				channel_id = i;
				break;
			}
		}
		assert(channel_id != -1);
		routing_table[n.index] = pair<int, int>(channel_id, 1);
		// cout << index << "\t" << routing_table.size() << endl;
		
		for (auto iter = routing_table.begin(); iter != routing_table.end();)
		{
		// cout << index << "\t" << routing_table.size() << endl;
			if (iter->second.first == channel_id)
			{
				int dist = n.get_logical_distance(index, iter->first);
				if (dist == -1)
				{
					// damaged route
					routing_table.erase(iter++);
				}
				else
				{
					iter++->second.second = dist + 1;
				}
			}
			else
			{
				++iter;
			}
		}
		for (auto iter : n.get_routing_table())
		{
		// cout << index << "\t" << routing_table.size() << endl;
			if (iter.first == index) continue;
			if (n.get_logical_distance(index, iter.first) == -1) continue;

			if (!routing_table.count(iter.first)
				|| routing_table[iter.first].second > iter.second.second + 1)
				{
				iter.second.first = channel_id;
				++iter.second.second; // ++distance
				if (iter.second.second <= Physics::MAX_HOP)
				{
					routing_table.insert(iter);
				}
			}
		}
		// cout << "END" << endl;
	}
	int Node::get_user_by_channel(int channel_id)
	{
		return channel_user[channel_id];
	}
	void Node::set_user_by_channel(int channel_id, int user_id)
	{
		channel_user[channel_id] = user_id;
	}
	bool Node::is_used_by_this_node(int channel_id)
	{
		return my_channel[channel_id];
	}
	void Node::set_used_by_this_node(int channel_id, bool flag)
	{
		my_channel[channel_id] = flag;
	}

	void Node::set_location(int x, int y)
	{
		Node::x = x;
		Node::y = y;
	}
	void Node::set_speed(int vx, int vy)
	{
		Node::vx = vx;
		Node::vy = vy;
	}
	void Node::move()
	{
		x += vx;
		y += vy;
	}
	int Node::distance_square(Node n)
	{
		int dx = n.x - x;
		int dy = n.y - y;
		return dx * dx + dy * dy;
	}
	int Node::get_logical_distance(int src, int node_id)
	{
		if (node_id == index)
		{
			return 0;
		}
		if (routing_table.count(node_id))
		{
			if (src == channel_user[routing_table[node_id].first])
			{
				return -1;
			}
			return routing_table[node_id].second;
		}
		return -1;
	}
	void Node::print()
	{
		cout << "Node " << index << ": (" << x << ',' << y << ") v=(" << vx << ',' << vy << ")" << endl;
		for (int i = 0; i < channel_cnt; ++i)
		{
			cout << '\t' << channel_user[i] << '(' << my_channel[i] << ')';
		}
		cout << endl;
	}
	void Node::print_channels()
	{
		for (int i = 0; i < channel_cnt; ++i)
		{
			if (my_channel[i])
			{
				cout << index << " => " << channel_user[i] << " (by channel " << i << ")" << endl;
			}
		}
	}
	void Node::print_routing_table()
	{
		cout << "node " << index << endl;
		for (auto iter : routing_table)
		{
			cout << "\tTo node " << iter.first <<
				"(by node " << channel_user[iter.second.first] <<
				",channel " << iter.second.first <<
				",distance " << iter.second.second << ")" << endl;
		}
	}
	Json::Value Node::routing_table_to_json()
	{
		Json::Value routes(Json::arrayValue);
		for (auto iter : routing_table)
		{
			Json::Value route;

			// dst(node_id) => pair(channel_id, distance)
			// map<int, pair<int, int> > routing_table;

			route["dest"] = iter.first;
			route["dist"] = iter.second.second;
			route["channel"] = iter.second.first;
			
			int user_id = channel_user[iter.second.first];
			if (user_id == -1)
			{
				continue;
			}
			route["via"] = user_id;
			
			routes.append(route);
		}
		return routes;

	}
	void Node::add_channels_to_result(vector<map<int, int> > &connected_nodes)
	{
		for (int i = 0; i < channel_cnt; ++i)
		{
			if (my_channel[i])
			{
				int min = index < channel_user[i] ? index : channel_user[i];
				int max = index > channel_user[i] ? index : channel_user[i];
				if (connected_nodes[i].count(min) && connected_nodes[i][min] != max)
				{
					cout << "CHANNEL ASSIGNMENT FAILURE" << endl;
				}
				else
				{
					connected_nodes[i][min] = max;
				}
			}
		}
	}
	int Node::get_index()
	{
		return index;
	}
	string Node::get_uuid()
	{
		return uuid;
	}

	void Node::output()
	{
		cerr << "Node " << index << ": (" << x << ',' << y << ") v=(" << vx << ',' << vy << ")" << endl;
		for (int i = 0; i < channel_cnt; ++i)
		{
			cerr << '\t' << channel_user[i] << '(' << my_channel[i] << ')';
		}
		cerr << endl;
	}
	void Node::output_channels()
	{
		for (int i = 0; i < channel_cnt; ++i)
		{
			if (my_channel[i])
			{
				cerr << index << " => " << channel_user[i] << " (by channel " << i << ")" << endl;
			}
		}
	}
	void Node::output_routing_table()
	{
		cerr << "node " << index << endl;
		for (auto iter : routing_table)
		{
			cerr << "\tTo node " << iter.first <<
				"(by node " << channel_user[iter.second.first] <<
				",channel " << iter.second.first <<
				",distance " << iter.second.second << ")" << endl;
		}
	}

	/* folloing methods for AODV */

	RouteRequest Node::creat_route_request(string destination_uuid)
	{
		assert(destination_uuid != "");

		RouteRequest rreq;
		rreq.set_source_uuid(uuid);
		rreq.set_broadcast_id(broadcast_id++);
		rreq.set_source_sequence_number(sequence_number++);
		rreq.set_destination_uuid(destination_uuid);
		rreq.set_destination_sequence_number(0);
		rreq.set_time_to_live(Physics::TIME_TO_LIVE);

		// auto p = make_pair(rreq, "");
		// request_records.push_back(p);

		sending_req_buffer.push_back(make_pair(rreq, ""));

		return rreq;
	};

	RouteRequest Node::creat_route_request(Physics::Capability capability)
	{
		RouteRequest rreq;
		rreq.set_source_uuid(uuid);
		rreq.set_broadcast_id(broadcast_id++);
		rreq.set_source_sequence_number(sequence_number++);
		rreq.set_target_capability(capability);
		rreq.set_destination_sequence_number(0);
		rreq.set_time_to_live(Physics::TIME_TO_LIVE);

		// auto p = make_pair(rreq, "");
		// request_records.push_back(p);

		sending_req_buffer.push_back(make_pair(rreq, ""));

		return rreq;
	};
	
	/* return RouteReply.time_to_live = 0 for not found */
	RouteReply Node::search_aodv_table(RouteRequest route_request)
	{
		int ttl = route_request.get_time_to_live();
		assert(ttl >= 0);

		string destination_uuid = route_request.get_destination_uuid();
		int destination_seq = route_request.get_destination_sequence_number();

		RouteReply rrep;
		rrep.set_time_to_live(0);

		if (destination_uuid == "")
		{
			// request for a target capability
			// but I have no idea about that
			return rrep;
		}

		for (auto iter : aodv_table)
		{
			if (iter.hit(destination_uuid, destination_seq))
			{
				rrep.set_source_uuid(route_request.get_source_uuid());
				rrep.set_destination_uuid(destination_uuid);
				rrep.set_destination_sequence_number(iter.get_sequence_number());
				rrep.set_hop_count(iter.get_hop_count());
				assert(iter.get_time_to_live() >= 0);
				rrep.set_time_to_live(iter.get_time_to_live());
				break;
			}
		}
		return rrep;
	}

	void Node::handle_route_request(RouteRequest route_request, string sender_uuid)
	{
		assert(sender_uuid != "");

		string destination_uuid = route_request.get_destination_uuid();
		if (destination_uuid == "")
		{
			// request for a target capability
			Physics::Capability capability = route_request.get_target_capability();
			assert(capability != Physics::Capability::NONE);
			
			if (has_capability(capability))
			{
				// I'm the node with the capability that you are looking for!
				RouteReply rrep;
				rrep.set_source_uuid(route_request.get_source_uuid());
				rrep.set_destination_uuid(uuid);
				rrep.set_target_capability(capability);
				rrep.set_destination_sequence_number(sequence_number++);
				rrep.set_hop_count(0);
				rrep.set_time_to_live(Physics::TIME_TO_LIVE);

				sending_rep_buffer.push_back(make_pair(rrep, sender_uuid));

				return;
			}
			// I don't have such capability
		}
		else if (destination_uuid == uuid)
		{
			// I'm the node you are looking for!
			RouteReply rrep;
			rrep.set_source_uuid(route_request.get_source_uuid());
			rrep.set_destination_uuid(uuid);
			rrep.set_destination_sequence_number(sequence_number++);
			rrep.set_hop_count(0);
			rrep.set_time_to_live(Physics::TIME_TO_LIVE);

			sending_rep_buffer.push_back(make_pair(rrep, sender_uuid));

			return;
		}

		RouteReply rrep = search_aodv_table(route_request);

		if (rrep.get_time_to_live() == 0)
		{
			// no fresh result in AODV table
			int ttl = route_request.get_time_to_live();
			if (ttl == 0)
			{
				return;
			}
			
			route_request.set_time_to_live(--ttl);
			
			// new broadcast
			sending_req_buffer.push_back(make_pair(route_request, ""));
		}
		else
		{
			// hit AODV table entry
			sending_rep_buffer.push_back(make_pair(rrep, sender_uuid));
		}
		
	};

	void Node::handle_route_reply(RouteReply route_reply, string sender_uuid)
	{
		string src_uuid = route_reply.get_source_uuid();
		string dst_uuid = route_reply.get_destination_uuid();

		if (src_uuid == uuid)
		{
			// the reply of my request
			AODVEntry entry(
				route_reply.get_destination_uuid(),
				sender_uuid,
				route_reply.get_destination_sequence_number(),
				route_reply.get_hop_count(),
				Physics::ENTRY_TIME
				);
			aodv_table.push_back(entry);
			
			return;
		}

		int ttl = route_reply.get_time_to_live();
		if (ttl == 0)
		{
			// discard the reply
			return;
		}

		int hop = route_reply.get_hop_count();
		route_reply.set_time_to_live(--ttl);
		route_reply.set_hop_count(++hop);

		string next_hop = "";
		for (auto iter : request_records)
		{
			RouteRequest rreq = iter.first;
			if (rreq.get_source_uuid() == src_uuid &&
				rreq.get_destination_uuid() == dst_uuid)
				{
				next_hop = iter.second;
				break;
			}
		}
		if (next_hop == "")
		{
			// don't know which request to reply
			// just discard the reply
			return;
		}
		
		sending_rep_buffer.push_back(make_pair(route_reply, next_hop));
	}

	void Node::handle_route_error(RouteError route_error, string sender_uuid)
	{
		assert(sender_uuid != "");

		string error_uuid = route_error.get_error_uuid();
		if (remove_aodv_entry_by_uuid(error_uuid))
		{
			// second scenario, need to broadcast
			sending_err_buffer.push_back(make_pair(route_error, ""));
		}
	}

	bool Node::remove_aodv_entry_by_uuid(string uuid)
	{
		assert(uuid != "");

		bool flag = false;
		for (auto iter : aodv_table)
		{
			if (iter.get_destination_uuid() == uuid)
			{
				// TODO: to remove or not to remove, that is the question
				flag = true;
			}
		}
		return flag;
	}


	Node::~Node()
	{
	}
