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

	void expand(string &str)
	{
		if (str == "i")
		{
			str = " { \"type\": \"init\", \"channelCount\" : 2, \"maxDistance\" : 10, \"nodes\" : [ \
				  {\"uuid\": \"uuid-01\", \"pos\" : [0, 0], \"speed\" : [0, 0]}, \
				  {\"uuid\": \"uuid-02\", \"pos\" : [0, 0], \"speed\" : [1, 0]}, \
				  {\"uuid\": \"uuid-03\", \"pos\" : [0, 0], \"speed\" : [4, 0]}, \
				  {\"uuid\": \"uuid-04\", \"pos\" : [0, 0], \"speed\" : [5, 0]}, \
				  {\"uuid\": \"uuid-05\", \"pos\" : [9, 0], \"speed\" : [5, 0]}, \
				  {\"uuid\": \"uuid-06\", \"pos\" : [18, 0], \"speed\" : [5, 0]} \
				  ]}";
		}
		else if (str == "u")
		{
			// update
			str = " { \"type\": \"update\", \"nodes\" : [ \
				  {\"uuid\": \"uuid-01\", \"speed\" : [0, 1]} \
				  ] }";
		}
		else if (str[0] == 'a')
		{
			// "a uuid-01"
			str = " {\"type\": \"nodeAdd\", \"nodes\": [ \
				  {\"uuid\": \"" + str.substr(2) + "\", \"pos\" : [10, 20], \"speed\" : [40, 50]} \
				  ]}";
		}
		else if (str[0] == 'r')
		{
			// "r uuid-01"
			str = " {\"type\": \"nodeRemove\", \"nodeIds\": [ \"" + str.substr(2) + "\" ]}";
		}
		else if (str == "l")
		{
			str = " {\"type\": \"listRoutes\"}";
		}


	}
	void print(Value obj)
	{
		// rm->print();
		cout << writer.write(obj);
	}

public:
	JsonHandler()
	{
		ok["status"] = "ok";
	}
	void run()
	{
		while (getline(cin, str))
		{
			expand(str);

			// cerr << "===============================" << endl;
			reader.parse(str, input);
			string type = input["type"].asString();
			if (type == "init")
			{
				rm = new RouteManager(input["channelCount"].asInt(), input["maxDistance"].asInt());
				cm = rm->get_channel_manager();
				nm = cm->get_node_manager();

				int node_cnt = input["nodes"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					Value new_node = input["nodes"][i];
					nm->add_node(
						new_node["uuid"].asString(),
						new_node["pos"][0].asInt(),
						new_node["pos"][1].asInt(),
						new_node["speed"][0].asInt(),
						new_node["speed"][1].asInt());
				}

				print(ok);
			}
			else if (type == "update")
			{
				int node_cnt = input["nodes"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					Value new_node = input["nodes"][i];
					nm->update_node(
						new_node["uuid"].asString(),
						new_node["speed"][0].asInt(),
						new_node["speed"][1].asInt());
				}
				rm->flush();

				print(cm->to_json());
			}
			else if (type == "nodeAdd")
			{
				int node_cnt = input["nodes"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					Value new_node = input["nodes"][i];
					nm->add_node(
						new_node["uuid"].asString(),
						new_node["pos"][0].asInt(),
						new_node["pos"][1].asInt(),
						new_node["speed"][0].asInt(),
						new_node["speed"][1].asInt());
				}

				print(ok);
			}
			else if (type == "nodeRemove")
			{
				int node_cnt = input["nodeIds"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					string node_id = input["nodeIds"][i].asString();
					nm->del_node(node_id);
				}

				print(ok);
			}
			else if (type == "listRoutes")
			{
				print(nm->routing_tables_to_json());
			}
			else
			{
				run_aodv();
			}
			// rm->output();
		}
	}

	void run_aodv()
	{
		while (getline(cin, str))
		{
			// expand(str);

			reader.parse(str, input);
			string type = input["type"].asString();
			if (type == "init")
			{
				rm = new RouteManager(input["channelCount"].asInt(), input["maxDistance"].asInt());
				cm = rm->get_channel_manager();
				nm = cm->get_node_manager();

				int node_cnt = input["nodes"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					Value new_node = input["nodes"][i];
					nm->add_node(
						new_node["uuid"].asString(),
						new_node["pos"][0].asInt(),
						new_node["pos"][1].asInt(),
						new_node["speed"][0].asInt(),
						new_node["speed"][1].asInt());
				}

				print(ok);
			}
			else if (type == "update")
			{
				int node_cnt = input["nodes"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					Value new_node = input["nodes"][i];
					nm->update_node(
						new_node["uuid"].asString(),
						new_node["speed"][0].asInt(),
						new_node["speed"][1].asInt());
				}
				rm->flush();

				print(cm->to_json());
			}
			else if (type == "nodeAdd")
			{
				int node_cnt = input["nodes"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					Value new_node = input["nodes"][i];
					nm->add_node(
						new_node["uuid"].asString(),
						new_node["pos"][0].asInt(),
						new_node["pos"][1].asInt(),
						new_node["speed"][0].asInt(),
						new_node["speed"][1].asInt());
				}

				print(ok);
			}
			else if (type == "nodeRemove")
			{
				int node_cnt = input["nodeIds"].size();
				for (int i = 0; i < node_cnt; ++i)
				{
					string node_id = input["nodeIds"][i].asString();
					nm->del_node(node_id);
				}

				print(ok);
			}
			else if (type == "listRoutes")
			{
				print(nm->routing_tables_to_json());
			}
			else
			{
				// wrong type
				assert(0);
			}
		}
	}

	virtual ~JsonHandler();
};
