#pragma once

#include <string>

#include "Physics.h"

using namespace std;

class RouteRequest
{
	string source_uuid;
	int broadcast_id;
	int source_sequence_number;

	string destination_uuid;
	Physics::Capability target_capability;
	int destination_sequence_number;

	// minus one each hop
	int time_to_live;

public:
	RouteRequest() : target_capability(Physics::Capability::NONE), time_to_live(-1)
	{};
	bool is_alive()
	{
		assert(time_to_live >= 0);
		return (time_to_live != 0);
	}

	string get_source_uuid()
	{
		return source_uuid;
	}
	void set_source_uuid(string uuid)
	{
		source_uuid = uuid;
	}

	int get_broadcast_id()
	{
		return broadcast_id;
	}
	void set_broadcast_id(int id)
	{
		broadcast_id = id;
	}

	int get_source_sequence_number()
	{
		return source_sequence_number;
	}
	void set_source_sequence_number(int seq)
	{
		source_sequence_number = seq;
	}

	string get_destination_uuid()
	{
		return destination_uuid;
	}
	void set_destination_uuid(string uuid)
	{
		destination_uuid = uuid;
	}

	Physics::Capability get_target_capability()
	{
		return target_capability;
	}
	void set_target_capability(Physics::Capability capability)
	{
		target_capability = capability;
	}

	int get_destination_sequence_number()
	{
		return destination_sequence_number;
	}
	void set_destination_sequence_number(int seq)
	{
		destination_sequence_number = seq;
	}

	int get_time_to_live()
	{
		return time_to_live;
	}
	void set_time_to_live(int time)
	{
		time_to_live = time;
	}

	~RouteRequest()
	{};
};