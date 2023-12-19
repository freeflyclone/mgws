#include "mgws.h"
#include "sessionmgr.h"

namespace {
	SessionID_t nextId{ 0 };
}

SessionManager g_sessions;

SessionManager::SessionPtr SessionManager::NewSession(Connection& c) {
	std::lock_guard<std::mutex> lock(m_idMutex);
	SessionPtr newSession = std::make_shared<Session>(nextId++, c);

	m_sessions.emplace(newSession->GetId(), newSession);

	return newSession;
}

bool SessionManager::AddSession(SessionPtr p) {
	try {
		std::lock_guard<std::mutex> lock(m_idMutex);

		p->SetId(nextId++);
		m_sessions.emplace(p->GetId(), p);
	}
	catch (std::exception& e) {
		TRACE("Exception: " << e.what());
		return false;
	}
	return true;
}
void SessionManager::DeleteSession(Session* session) {
	if (!session)
		return;

	if (m_sessions.empty())
		return;

	auto it = m_sessions.find(session->GetId());

	if (it == m_sessions.end())
		return;

	auto id = it->second->GetId();

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
			{"sessionId", session->GetId() },
			{"userName", session->GetUserName() },
		};
		msg["sessions"].push_back(s);
	});

	g_sessions.Iterate([&](Session* session) {
		session->Send(msg);
	});
}