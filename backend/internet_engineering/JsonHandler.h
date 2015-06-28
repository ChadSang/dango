#pragma once

#include <iostream>

#include "RouteManager.h"

using namespace Json;
using namespace std;

class JsonHandler
{
	Reader reader;
	FastWriter writer;

	string str;
	Value input;
	Value ok;

	RouteManager* rm;
	ChannelManager* cm;
	NodeManager* nm;

	void expand(string &str);
	void expand_test(string &str);
	void print(Value obj);

public:
	JsonHandler();

	void run();
	void run_aodv();

	virtual ~JsonHandler();
};
