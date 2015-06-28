#include "AODVEntry.h"

	AODVEntry::AODVEntry() : time_to_live(-1)
	{};
	AODVEntry::AODVEntry(string dst_uuid, string next_hop_uuid, int seq, int hop_count, int time_to_live) :
		destination_uuid(dst_uuid), next_hop_uuid(next_hop_uuid),
		sequence_number(seq), hop_count(hop_count), time_to_live(time_to_live)
	{};
	bool AODVEntry::hit(string dst_uuid, int seq)
	{
		assert(time_to_live > 0);
		if (dst_uuid == destination_uuid)
		{
			if (seq <= sequence_number)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	string AODVEntry::get_destination_uuid()
	{
		return destination_uuid;
	}
	void AODVEntry::set_destination_uuid(string dst_uuid)
	{
		assert(dst_uuid != "");
		destination_uuid = dst_uuid;
	}
	string AODVEntry::get_destination_capability()
	{
		return destination_capability;
	}
	void AODVEntry::set_destination_capability(string destination_capability)
	{
		this->destination_capability = destination_capability;
	}

	string AODVEntry::get_next_hop_uuid()
	{
		return next_hop_uuid;
	}
	void AODVEntry::set_next_hop_uuid(string next)
	{
		assert(next != "");
		next_hop_uuid = next;
	}

	int AODVEntry::get_sequence_number()
	{
		return sequence_number;
	}
	void AODVEntry::set_sequence_number(int seq)
	{
		sequence_number = seq;
	}

	int AODVEntry::get_hop_count()
	{
		return hop_count;
	}
	void AODVEntry::set_hop_count(int hop)
	{
		hop_count = hop;
	}

	int AODVEntry::get_time_to_live()
	{
		return time_to_live;
	}
	void AODVEntry::set_time_to_live(int ttl)
	{
		assert(ttl > 0);
		time_to_live = ttl;
	}

	AODVEntry::~AODVEntry()
	{}

