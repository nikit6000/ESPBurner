#include "StdAfx.h"
#include "WebHandler.h"


WebHandler::WebHandler()
{
	//ctor
}
WebHandler::WebHandler(utility::string_t url) :m_listener(url)
{
	m_listener.support(methods::GET, std::bind(&WebHandler::handle_get, this, std::placeholders::_1));
	m_listener.support(methods::PUT, std::bind(&WebHandler::handle_put, this, std::placeholders::_1));
	m_listener.support(methods::POST, std::bind(&WebHandler::handle_post, this, std::placeholders::_1));
	m_listener.support(methods::DEL, std::bind(&WebHandler::handle_delete, this, std::placeholders::_1));

}
WebHandler::~WebHandler()
{
	//dtor
}

void WebHandler::setUpdatingStatus(bool status)
{
	isDataUpdating = status;
	/*if (!status) {
		for (int i = 0; i < rqQueue.size(); i++) {
			handle_get(rqQueue[i]);
		}
	}*/
}

void WebHandler::handle_error(pplx::task<void>& t)
{
	try
	{
		t.get();
	}
	catch (...)
	{
		// Ignore the error, Log it if a logger is available
	}
}


//
// Get Request 
//
void WebHandler::handle_get(http_request message)
{
	try {
		ucout << message.to_string() << endl;

		auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
		string_t path = message.relative_uri().path();

		auto query = http::uri::split_query(message.relative_uri().query());

		bool full = query.find(L"full") != query.end() ? query[U("full")] == L"1" : false;

		if (isDataUpdating)
		{
			//rqQueue.push_back(message);
			message.reply(status_codes::NoContent, L"{\"error\":\"Data updating\"}", L"application/json").then([](pplx::task<void> t)
			{
				try {
					t.get();
				}
				catch (...) {
					//
				}
			});
			return;
		}

		if (nodes.find(query[U("node")]) != nodes.end()) {
			auto jsonString = nodes[query[L"node"]]->toJSON(full).serialize();
			if (nodes[query[L"node"]]->selfDistruct)
				delete nodes[query[L"node"]];
			message.reply(status_codes::OK, jsonString, L"application/json").then([](pplx::task<void> t)
			{
				try {
					t.get();
				}
				catch (...) {
					//
				}
			});
		}
		else {
			message.reply(status_codes::NotFound, L"{\"error\":\"Not found\"}", L"application/json").then([](pplx::task<void> t)
			{
				try {
					t.get();
				}
				catch (...) {
					//
				}
			});
		}


	}
	catch (...) {

	}
	return;

};

//
// A POST request
//
void WebHandler::handle_post(http_request message)
{
	ucout << message.to_string() << endl;


	message.reply(status_codes::OK, message.to_string());
	return;
};

//
// A DELETE request
//
void WebHandler::handle_delete(http_request message)
{
	ucout << message.to_string() << endl;

	string rep = "WRITE YOUR OWN DELETE OPERATION";
	message.reply(status_codes::OK, rep);
	return;
};


//
// A PUT request 
//
void WebHandler::handle_put(http_request message)
{
	ucout << message.to_string() << endl;
	string rep = "WRITE YOUR OWN PUT OPERATION";
	message.reply(status_codes::OK, rep);
	return;
};