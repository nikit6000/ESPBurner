#include "stdafx.h"
#include "NodeObject.h"

NodeObject::NodeObject()
{
	this->name = L"default";
}

NodeObject::NodeObject(string_t name)
{
	this->name = name;
}

NodeObject::~NodeObject()
{
}

json::value NodeObject::toJSON(bool full)
{
	busy = true;
	json::value object;
	object[L"framerate"] = json::value::number(framerate);
	if (full) {
		std::vector<json::value> JSON_payload;
		for (int i = 0; i < this->payload.size(); i++) {
			if (i < this->payload.size()) {
				printf("NodeObject: index %d of payload %d", i, this->payload.size());
				json::value item;
				item[L"name"] = json::value::string(this->payload[i].name);
				item[L"value"] = json::value::number(this->payload[i].value);
				item[L"unit"] = json::value::string(this->payload[i].unit);
				item[L"flags"] = json::value::number((int)(this->payload[i].flags));
				JSON_payload.push_back(item);
			}
		}
		if (JSON_payload.size() > 0)
			object[L"payload"] = json::value::array(JSON_payload);
		else
			printf("Wows\n");
	}
	busy = false;
	return object;
}

void NodeObject::processEntry(LPMAHM_SHARED_MEMORY_ENTRY lpEntry)
{
	if (strcmp(lpEntry->szSrcName, "Framerate") == 0) {
		framerate = lpEntry->data == FLT_MAX ? 0 : lpEntry->data;
		return;
	}

	NodePayload gpuPayload;
	if (lpEntry->data == FLT_MAX)
		gpuPayload.value = -1.0;
	else
	{
		gpuPayload.value = lpEntry->data;

		if (lpEntry->szSrcUnits) {
			const size_t cSize = strlen(lpEntry->szSrcUnits);
			std::wstring wc(cSize, L'#');
			mbstowcs(&wc[0], lpEntry->szSrcUnits, cSize);
			gpuPayload.unit = string_t(wc);
		}
	}
	const size_t szSrcNameSize = strlen(lpEntry->szSrcName);
	std::wstring szSrcName(szSrcNameSize, L'#');
	mbstowcs(&szSrcName[0], lpEntry->szSrcName, szSrcNameSize);

	

	gpuPayload.name = string_t(szSrcName);
	gpuPayload.flags = lpEntry->dwFlags;

	

	this->payload.push_back(gpuPayload);

}

void NodeObject::fromNode(NodeObject obj)
{
	this->payload.clear();
	this->payload = obj.payload;
}
