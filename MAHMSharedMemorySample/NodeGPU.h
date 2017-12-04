#pragma once
#include "NodeObject.h"

class NodeGPU: public NodeObject
{
private:
	int index;
public:
	string_t device, family, driver, bios;
	int memory;
	NodeGPU();
	NodeGPU(int index, LPMAHM_SHARED_MEMORY_GPU_ENTRY lpGpuEntry);
	~NodeGPU();
	json::value toJSON(bool full = false);
};

