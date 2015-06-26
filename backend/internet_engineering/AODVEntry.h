#pragma once

#include <string>
#include <cassert>

using namespace std;

class AODVEntry
{
	string destination_uuid;
	string next_hop_uuid;

	int sequence_number;
	int hop_count;

	// minus one each update
	int time_to_live;

public:
	AODVEntry() {};
	AODVEntry(string dst_uuid, string next_hop_uuid, int seq, int hop_count, int time_to_live) : 
		destination_uuid(dst_uuid), next_hop_uuid(next_hop_uuid),
		sequence_number(seq), hop_count(hop_count), time_to_live(time_to_live) {};
	bool hit(string dst_uuid, int seq) {
		assert(time_to_live > 0);
		if (dst_uuid == destination_uuid) {
			if (seq <= sequence_number) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	string get_destination_uuid() {
		return destination_uuid;
	}
	void set_destination_uuid(string dst_uuid) {
		assert(dst_uuid != "");
		destination_uuid = dst_uuid;
	}

	string get_next_hop_uuid() {
		return next_hop_uuid;
	}
	void set_next_hop_uuid(string next) {
		assert(next != "");
		next_hop_uuid = next;
	}

	int get_sequence_number() {
		return sequence_number;
	}
	void set_sequence_number(int seq) {
		sequence_number = seq;
	}

	int get_hop_count() {
		return hop_count;
	}
	void set_hop_count(int hop) {
		hop_count = hop;
	}

	int get_time_to_live() {
		return time_to_live;
	}
	void set_time_to_live(int ttl) {
		assert(ttl > 0);
		time_to_live = ttl;
	}

	~AODVEntry() {};
};
