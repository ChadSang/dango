#pragma once

#include <string>
#include <cassert>

using namespace std;

class AODVEntry
{
	string destination_uuid;
	string destination_capability;
	string next_hop_uuid;

	int sequence_number;
	int hop_count;

	// minus one each update
	int time_to_live;

public:
	AODVEntry();
	AODVEntry(string dst_uuid, string next_hop_uuid, int seq, int hop_count, int time_to_live);
	bool hit(string dst_uuid, int seq);

	string get_destination_uuid();
	void set_destination_uuid(string dst_uuid);
	string get_destination_capability();
	void set_destination_capability(string destination_capability);

	string get_next_hop_uuid();
	void set_next_hop_uuid(string next);

	int get_sequence_number();
	void set_sequence_number(int seq);

	int get_hop_count();
	void set_hop_count(int hop);

	int get_time_to_live();
	void set_time_to_live(int ttl);

	~AODVEntry();
};
