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

void SessionManager::UpdateSession(const uint32_t id, const std::string& userName, const std::string& localId) {
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

SessionManager::SessionPtr SessionManager::GetSessionById(const SessionID_t id)
{
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
	webrtc::sessionsChanged::Message msg;
	msg.type = "SessionsChanged";

	g_sessions.Iterate([&](Session* session) {
		msg.sessions.push_back({ session->getId(), session->UserName(), session->LocalId() });
	});

	g_sessions.Iterate([&](Session* session) {
		session->Send(msg);
	});
}