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

	UpdateSessionsList();
}

void SessionManager::UpdateSession(const uint32_t id, const std::string& userName) {
	auto sessPair = m_sessions.find(id);
	if (sessPair == m_sessions.end()) {
		TRACE("Oops: didn't find g_sessions[" << id << "]");
		return;
	}

	auto session = (sessPair->second);

	// used by Peer::OnLocalIdEvent() to update g_sessions[m_id]
	session->SetUserName(userName);
}

SessionManager::SessionPtr SessionManager::GetSessionById(const std::string& sessId)
{
	const SessionID_t id = static_cast<SessionID_t>(std::stoi(sessId));

	auto sessPair = m_sessions.find(id);
	if (sessPair == m_sessions.end()) {
		TRACE("Oops: didn't find g_sessions[" << id << "]");
		return nullptr;
	}
	return sessPair->second;
}

void SessionManager::Iterate(SessionCallback_fn fn) {
	for (auto p : m_sessions) {
		fn(p.second.get());
	}
}

void SessionManager::UpdateSessionsList() {
	json msg = { { "type", "SessionsChanged"} };

	g_sessions.Iterate([&](Session* session) {
		json s = {
			{"sessionId", session->getId() },
			{"userName", session->UserName() },
		};
		msg["sessions"].push_back(s);
	});

	g_sessions.Iterate([&](Session* session) {
		session->Send(msg);
	});
}