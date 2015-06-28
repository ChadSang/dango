#pragma once

#include <cassert>

class Physics
{
public:
	static const int TIME_TO_LIVE = 10;
	static const int ENTRY_TIME = 100;
	static const int MAX_HOP = 5;

	enum Capability { NONE = 0, Red, Blue, Green };

	Physics();
	~Physics();
};

