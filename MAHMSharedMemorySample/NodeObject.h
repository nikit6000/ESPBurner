#pragma once
#include "StdAfx.h"
#include "MAHMSharedMemory.h"
#include <locale>
#include <codecvt>

using namespace utility;
using namespace web;

struct NodePayload
{
	string_t name;
	float value;
	string_t unit;
	DWORD flags;
};

class NodeObject
{
private:
	int framerate;
	bool busy = false;
protected:
	string_t name;
public:
	bool selfDistruct = false;

	std::vector<NodePayload> payload;
	NodeObject();
	NodeObject(string_t name);
	~NodeObject();
	string_t getName() { return name; }
	virtual json::value toJSON(bool full = false);
	void processEntry(LPMAHM_SHARED_MEMORY_ENTRY lpEntry);
	void fromNode(NodeObject obj);
	bool getBusy() { return busy; }
};

