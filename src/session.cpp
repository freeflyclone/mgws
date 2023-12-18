#include <iostream>

#include "mgws.h"
#include "session.h"

Session::Session(SessionID_t id, Connection& c)
	: m_id(id),
	m_connection(c),
	m_userName()
{
	m_connection.fn_data = this;

	Send({ {"type", "SessionID"}, {"id", m_id } });
}

Session::~Session() 
{
}

void Session::OnMessage(Message* msg) {
	try {
		std::string msgString(msg->data.ptr, msg->data.len);

		TRACE("OnMessage: " << msgString);

		// TODO pass to app somehow

		json j = json::parse(msgString);
	}
	catch (std::exception& e) {
		TRACE("OnMessage exception: " << e.what());
	}
}

void Session::Send(const json& msg) {
	try {
		std::string msg_str(msg.dump());
		mg_ws_send((struct mg_connection*)&m_connection, msg_str.c_str(), msg_str.length(), WEBSOCKET_OP_TEXT);
	}
	catch (std::exception& e) {
		TRACE("Send() exception:  " << e.what());
	}
}

