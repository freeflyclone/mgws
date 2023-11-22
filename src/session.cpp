#include <iostream>

#include "mgws.h"
#include "session.h"

namespace {
	SessionID_t nextId{ 0 };
	SessionsList g_sessions;
}

Session::Session(Connection& c)
	: m_connection(c),
	m_id(nextId++)
{
	TRACE("Session(" << m_id << ")");

	m_connection.fn_data = this;
}

Session::~Session() 
{
	TRACE("~Session(" << m_id << ")");
}

void Session::OnMessage(Message* msg) {
	std::string message(msg->data.ptr, msg->data.len);

	json j = json::parse(message);

	TRACE(__FUNCTION__ << ", type: " << j["type"]);
}

SessionPtr NewSession(Connection& c) {
	SessionPtr newSession = std::make_shared<Session>(c);

	g_sessions.emplace(newSession->getId(), newSession);

	return newSession;
}

void DeleteSession(Session* session) {
	if (!session)
		return;

	if (g_sessions.empty())
		return;

	auto it = g_sessions.find(session->getId());

	if (it == g_sessions.end())
		return;

	auto id = it->second->getId();

	TRACE("Deleting id: " << id);

	g_sessions.erase(id);
}