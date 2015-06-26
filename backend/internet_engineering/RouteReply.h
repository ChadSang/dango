#pragma once

#include <string>

#include "Physics.h"

using namespace std;

class RouteReply
{
	string source_uuid;
	string destination_uuid;
	Physics::Capability target_capability;

	int destination_sequence_number;
	int hop_count;

	// minus one each hop
	int time_to_live;

public:
	RouteReply() : time_to_live(-1) {};

	bool is_alive() {
		assert(time_to_live >= 0);
		return (time_to_live != 0);
	}

	string get_source_uuid() {
		return source_uuid;
	}
	void set_source_uuid(string uuid) {
		source_uuid = uuid;
	}

	string get_destination_uuid() {
		return destination_uuid;
	}
	void set_destination_uuid(string uuid) {
		destination_uuid = uuid;
	}

	Physics::Capability get_target_capability() {
		return	target_capability;
	}
	void set_target_capability(Physics::Capability capability) {
		target_capability = capability;
	}

	int get_destination_sequence_number() {
		return destination_sequence_number;
	}
	void set_destination_sequence_number(int seq) {
		destination_sequence_number = seq;
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
	void set_time_to_live(int time) {
		time_to_live = time;
	}
	~RouteReply() {};
};

