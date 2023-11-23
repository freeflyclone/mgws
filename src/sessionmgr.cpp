#include "mgws.h"
#include "sessionmgr.h"

namespace {
	SessionID_t nextId{ 0 };
}

SessionManager g_sessions;

SessionManager::SessionPtr SessionManager::NewSession(Connection& c) {
	SessionPtr newSession = std::make_shared<Session>(nextId++, c);

	m_sessions.emplace(newSession->getId(), newSession);

	return newSession;
}

void SessionManager::DeleteSession(Session* session) {
	if (!session)
		return;

	if (m_sessions.empty())
		return;

	auto it = m_sessions.find(session->getId());

	if (it == m_sessions.end())
		return;

	auto id = it->second->getId();

	TRACE("Deleting id: " << id);

	m_sessions.erase(id);
}