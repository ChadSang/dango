#pragma once

#include <cassert>

class Physics
{
public:
	static const int TIME_TO_LIVE = 10;
	static const int ENTRY_TIME = 100;

	enum Capability { NONE = 0, First, Second };

	Physics();
	~Physics();
};

