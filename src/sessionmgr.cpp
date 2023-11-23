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

void SessionManager::UpdateSessions(const uint32_t id, const std::string& userName, const std::string& localId) {
	auto sessPair = m_sessions.find(id);
	if (sessPair == m_sessions.end()) {
		TRACE("Oops: didn't find g_sessions[" << id << "]");
		return;
	}

	auto session = (sessPair->second);

	// use "localIdEvent" to update g_sessions[m_id]
	session->SetUserName(userName);
	session->SetLocalId(localId);
}
