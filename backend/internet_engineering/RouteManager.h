#pragma once

#include "ChannelManager.h"

using namespace std;

class RouteManager
{
	ChannelManager* channels;

public:
	RouteManager(int channel_cnt, int radius) :
		channels(new ChannelManager(channel_cnt, radius)) {
	}
	void flush() {
		channels->flush();
		// cout << "route: flushing..." << endl;
	}
	void run() {
		print();
		string command;
		while (getline(cin, command)) {
			if (command == "q") return;
			flush();
			print();
		}
	}
	void print() {
		channels->print();
		// cout << "Route: " << endl;
	}

	void output() {
		channels->output();
	}
	ChannelManager* get_channel_manager() {
		return channels;
	}

	virtual ~RouteManager();
};

