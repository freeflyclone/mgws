#include "mgws.h"
#include "sessionmgr.h"

SessionManager::SessionManager(
	const std::string& root,
	const std::string& cert_name,
	const std::string& key_name) 
	: mgws(root, cert_name, key_name),
	m_nextId(0)
{
}

void SessionManager::fn(struct mg_connection* c, int ev, void* ev_data, context* ctx)
{
	if (MG_EV_ACCEPT == ev) {
		mg_tls_init(c, &m_tls_opts);
		return;
	}

	if (MG_EV_HTTP_MSG == ev) {
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;

		if (mg_http_match_uri(hm, "/websock")) {
			// upgrade to ws:/wss: connection, recv MG_EV_WS_MSG thereafter
			mg_ws_upgrade(c, hm, NULL);

			AddSession(std::make_shared<Session>(ctx, *c));
		}
		else {
			mg_http_serve_dir(c, hm, &m_http_serve_opts);
		}
		return;
	}

	if (MG_EV_WS_MSG == ev) {
		((Session*)(ctx->user_data))->OnMessage((Message*)ev_data);
		return;
	}

	if (MG_EV_CLOSE == ev) {
		DeleteSession((Session*)ctx->user_data);
		return;
	}
}

bool SessionManager::AddSession(SessionPtr p) {
	try {
		std::lock_guard<std::mutex> lock(m_idMutex);

		p->SetId(m_nextId++);
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
		TRACE("Oops: didn't find Session [" << id << "]");
		return;
	}

	auto session = (sessPair->second);

	session->SetUserName(userName);
}

SessionManager::SessionPtr SessionManager::GetSessionById(const std::string& sessId)
{
	const SessionID_t id = static_cast<SessionID_t>(std::stoi(sessId));

	auto sessPair = m_sessions.find(id);
	if (sessPair == m_sessions.end()) {
		TRACE("Oops: didn't find Session [" << id << "]");
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

	Iterate([&](Session* session) {
		json s = {
			{"sessionId", session->GetId() },
			{"userName", session->GetUserName() },
		};
		msg["sessions"].push_back(s);
	});

	Iterate([&](Session* session) {
		session->Send(msg);
	});
}