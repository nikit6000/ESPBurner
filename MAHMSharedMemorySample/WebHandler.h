#pragma once

#include "stdafx.h"
#include <regex>
#include <map>
#include <vector>
#include "NodeObject.h"
#include "NodeGPU.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;


class WebHandler
{
public:
	std::map<string_t, NodeObject *> nodes;
	
	WebHandler();
	WebHandler(utility::string_t url);
	virtual ~WebHandler();

	pplx::task<void>open() { return m_listener.open(); }
	pplx::task<void>close() { return m_listener.close(); }
	void setUpdatingStatus(bool status);

protected:

private:
	bool isDataUpdating = false;
	vector<http_request> rqQueue;
	void handle_get(http_request message);
	void handle_put(http_request message);
	void handle_post(http_request message);
	void handle_delete(http_request message);
	void handle_error(pplx::task<void>& t);
	http_listener m_listener;
};
