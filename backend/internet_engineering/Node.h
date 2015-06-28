#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "json/json.h"
#include "json/json-forwards.h"

#include "Physics.h"

#include "AODVEntry.h"
#include "RouteRequest.h"
#include "RouteError.h"
#include "RouteReply.h"

using namespace std;

class NodeManager;

class Node
{
	int index;
	const string uuid;

	// location and velocity
	int x;
	int y;
	int vx;
	int vy;

	set<Physics::Capability> capabilities;

	int channel_cnt;

	// current user's ID(node) of each channel, -1 for unknown
	vector<int> channel_user;

	// if the channel is used by this node
	vector<bool> my_channel;

	// if I'm the master of the channel
	vector<bool> master_user;

	// dst(node_id) => pair(channel_id, distance)
	map<int, pair<int, int> > routing_table;
	
	/* following attributes for AODV */
	int sequence_number;
	int broadcast_id;
	
	vector<AODVEntry> aodv_table;
	vector<pair<RouteRequest, string> > request_records; // pair(req, sender_uuid)

public:
	vector<pair<RouteRequest, string> > received_req_buffer;
	vector<pair<RouteReply, string> > received_rep_buffer;
	vector<pair<RouteError, string> > received_err_buffer;

	vector<pair<RouteRequest, string> > sending_req_buffer;
	vector<pair<RouteReply, string> > sending_rep_buffer;
	vector<pair<RouteError, string> > sending_err_buffer;

	Node(int index, int channel_cnt);
	Node(int index, int x, int y, int vx, int vy, int channel_cnt);
	Node(int index, string uuid, int x, int y, int vx, int vy, int channel_cnt);

	void add_capability(Physics::Capability capability);
	bool has_capability(Physics::Capability capability);
	void remove_capability(Physics::Capability capability);

	void check_channels(const multimap<int, int> &communicable_nodes);
	void check_routing_table();
	const map<int, pair<int, int> > get_routing_table();

	void receive(Node &n);
	bool channel_handler(Node &n);
	void route_handler(Node n);

	int get_user_by_channel(int channel_id);
	void set_user_by_channel(int channel_id, int user_id);
	bool is_used_by_this_node(int channel_id);
	void set_used_by_this_node(int channel_id, bool flag);

	void set_location(int x, int y);
	void set_speed(int vx, int vy);
	void move();

	int distance_square(Node n);
	int get_logical_distance(int src, int node_id);

	void print();
	void print_channels();
	void print_routing_table();

	Json::Value routing_table_to_json();
	void add_channels_to_result(vector<map<int, int> > &connected_nodes);
	int get_index();
	string get_uuid();

	void output();
	void output_channels();
	void output_routing_table();

	/* folloing methods for AODV */

	RouteRequest creat_route_request(string destination_uuid);
	RouteRequest creat_route_request(Physics::Capability capability);
	
	/* return RouteReply.time_to_live = 0 for not found */
	RouteReply search_aodv_table(RouteRequest route_request);

	void handle_route_request(RouteRequest route_request, string sender_uuid);
	void handle_route_reply(RouteReply route_reply, string sender_uuid);
	void handle_route_error(RouteError route_error, string sender_uuid);

	bool remove_aodv_entry_by_uuid(string uuid);

	virtual ~Node();
};
