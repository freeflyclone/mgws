#include <iostream>

#include "session.h"
namespace {
	SessionID_t nextId{ 0 };
	SessionsList g_sessions;
}


Session::Session(Connection& c)
	: m_connection(c),
	m_id(nextId++)
{
	std::clog << "Session(" << m_id << ")" << std::endl;
	m_connection.fn_data = this;
}

Session::~Session() 
{
	std::clog << "~Session(" << m_id << ")" << std::endl;
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

	std::cout << "Deleting id: " << id << std::endl;

	g_sessions.erase(id);
}