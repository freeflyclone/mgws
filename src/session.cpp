#include <iostream>

#include "mgws.h"
#include "session.h"

Session::Session(mgws::context* ctx, Connection& c)
	: m_connection(c),
	m_userName(),
	m_ctx({ctx->_mgws, this}),
	m_lastPongTime(mg_millis())
{
	// MUST set our connection's fn_data with ptr to
	// this->m_ctx so mongoose event handler knows
	// which Session it's talking to.
	m_connection.fn_data = &m_ctx;

	//TRACE(__FUNCTION__ << "(): " << m_id);
}

Session::~Session()
{
	//TRACE(__FUNCTION__);
}

void Session::OnTimerEvent(int64_t ms) {
	mg_ws_send(&m_connection, "HEARTBEAT", 10, WEBSOCKET_OP_PING);
}

SessionID_t Session::GetId() {
	return static_cast<SessionID_t>(m_connection.id & (SessionID_t)-1);
};

const std::string& Session::GetUserName() {
	return m_userName;
}

void Session::SetUserName(const std::string& name) {
	m_userName = name;
}

void Session::OnControlMessage(Message* msg) {
	try {
		auto op = msg->flags & 0xF;
		switch (op) {
			case WEBSOCKET_OP_PONG:
				{
					m_lastPongTime = mg_millis();
					std::string msgString(msg->data.ptr, msg->data.len);
				}
				break;

			default:
				break;
		}
	}
	catch (std::exception& e) {
		TRACE("OnControlMessage exception: " << e.what());
	}
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


