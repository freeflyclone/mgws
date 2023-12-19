#include <iostream>

#include "mgws.h"
#include "session.h"

Session::Session(Connection& c)
	: m_id((SessionID_t)-1),
	m_connection(c),
	m_userName()
{
	m_connection.fn_data = this;
}

Session::Session(SessionID_t id, Connection& c)
	: Session(c)
{
	m_id = id;
	Send({ {"type", "SessionID"}, {"id", m_id } });

	return;
}

Session::~Session()
{
}

SessionID_t Session::GetId() {
	return m_id;
};

void Session::SetId(SessionID_t id)
{
	// only set m_id once!
	if (m_id == (SessionID_t)-1) {
		m_id = id;
		Send({ {"type", "SessionID"}, {"id", m_id } });
	}
}

const std::string& Session::GetUserName() {
	return m_userName;
}

void Session::SetUserName(const std::string& name) {
	m_userName = name;
}

void Session::OnMessage(Message* msg) {
	try {
		std::string msgString(msg->data.ptr, msg->data.len);
		TRACE("OnMessage: " << msgString);
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

