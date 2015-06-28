#pragma once

#include <string>

using namespace std;

class RouteError
{
	string error_uuid;

	// minus one each hop
	int time_to_live;

public:
	RouteError() : time_to_live(-1)
	{};
	string get_error_uuid()
	{
		return error_uuid;
	}
	void set_error_uuid(string uuid)
	{
		error_uuid = uuid;
	}

	int get_time_to_live()
	{
		return time_to_live;
	}
	void set_time_to_live(int ttl)
	{
		time_to_live = ttl;
	}

	~RouteError()
	{};
};

