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
	TRACE("Session(" << m_id << ")");

	m_connection.fn_data = this;

	json sessionId;
	sessionId["type"] = "SessionID";
	sessionId["id"] = m_id;

	Send(sessionId);
}

Session::~Session() 
{
	TRACE("~Session(" << m_id << ")");
}

void Session::OnMessage(Message* msg) {
	try {
		m_peer.HandleMessage(json::parse(std::string(msg->data.ptr, msg->data.len)));
	}
	catch (std::exception& e) {
		TRACE("Exception " << e.what());
	}
}

void Session::Send(const json& msg) {
	std::string msg_str(msg.dump());

	TRACE(__FUNCTION__ << "(), id: " << m_id <<  ", type: " << msg["type"]);

	mg_ws_send((struct mg_connection*)&m_connection, msg_str.c_str(), msg_str.length(), WEBSOCKET_OP_TEXT);
}

