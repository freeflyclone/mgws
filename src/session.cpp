#include <iostream>

#include "mgws.h"
#include "session.h"

Session::Session(uint32_t id, Connection& c)
	: m_id(id),
	m_connection(c),
	m_peer(this),
	m_localId(),
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
		json j = json::parse(std::string(msg->data.ptr, msg->data.len));
		m_peer.HandleMessage(j);
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

