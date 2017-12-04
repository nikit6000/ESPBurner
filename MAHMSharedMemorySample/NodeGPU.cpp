#include "stdafx.h"
#include "NodeGPU.h"


NodeGPU::NodeGPU()
{
}

NodeGPU::NodeGPU(int index, LPMAHM_SHARED_MEMORY_GPU_ENTRY lpGpuEntry) : NodeObject(U("gpu")+conversions::to_string_t(std::to_string(index)))
{
	this->index = index;

	if (strlen(lpGpuEntry->szDevice))
	{
		CString strDevice;
		device = conversions::to_string_t(std::string(lpGpuEntry->szDevice));

		if (strlen(lpGpuEntry->szFamily)) 
			family = conversions::to_string_t(std::string(lpGpuEntry->szFamily));

	}

	if (strlen(lpGpuEntry->szDriver))
		driver = conversions::to_string_t(std::string(lpGpuEntry->szDriver));


	if (strlen(lpGpuEntry->szBIOS))
		bios = conversions::to_string_t(std::string(lpGpuEntry->szBIOS));


	if (lpGpuEntry->dwMemAmount)
		memory = lpGpuEntry->dwMemAmount / 1024;

}

NodeGPU::~NodeGPU()
{
}

json::value NodeGPU::toJSON(bool full)
{
	json::value object;
	object[L"index"] = json::value::number(index);
	object[L"device"] = json::value::string(device);
	object[L"family"] = json::value::string(family);
	object[L"driver"] = json::value::string(driver);
	object[L"bios"] = json::value::string(bios);
	object[L"mem"] = json::value::number(memory);
	if (full && this->payload.size() > 0) {
		std::vector<json::value> payload;
		for (int i = 0; i < this->payload.size(); i++) {
			printf("NodeGPU: index %d of payload %d", i, this->payload.size());
			json::value item;
			item[L"name"] = json::value::string(this->payload[i].name);
			item[L"value"] = json::value::number(this->payload[i].value);
			item[L"unit"] = json::value::string(this->payload[i].unit);
			item[L"flags"] = json::value::number((int)this->payload[i].flags);
			payload.push_back(item);
		}
		if (payload.size() > 0)
			object[L"payload"] = json::value::array(payload);
	}
	return object;
}
