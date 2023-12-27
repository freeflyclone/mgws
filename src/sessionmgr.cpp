#include "mgws.h"
#include "sessionmgr.h"

SessionManager::SessionManager(
	const std::string& root,
	const std::string& cert_name,
	const std::string& key_name) 
	: mgws(root, cert_name, key_name),
	m_factory([](mgws::context* ctx, Connection& c) -> SessionPtr { return new Session(ctx, c); })
{
}

void SessionManager::SetFactory(SessionFactory_t fn) {
	m_factory = fn;
}

void SessionManager::timer_event(int64_t ms)
{
	Iterate([&](Session* session) {
		session->OnTimerEvent(ms);
	});
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

			AddSession(ctx, c);
		}
		else {
			mg_http_serve_dir(c, hm, &m_http_serve_opts);
		}
		return;
	}

	if (MG_EV_WS_MSG == ev) {
		auto session = (Session*)(ctx->user_data);
		if (session == nullptr) {
			TRACE(__FUNCTION__ << "() Oops, ctx->user_data is null");
			return;
		}

		session->OnMessage((Message*)ev_data);

		return;
	}

	if (MG_EV_WS_CTL == ev) {
		auto session = (Session*)(ctx->user_data);
		if (session == nullptr) {
			TRACE(__FUNCTION__ << "() Oops, ctx->user_data is null");
			return;
		}
		session->OnControlMessage((Message*)ev_data);
		return;
	}

	if (MG_EV_CLOSE == ev) {
		auto session = (Session*)(ctx->user_data);
		if (session == nullptr) {
			return;
		}

		DeleteSession(session);
		return;
	}
}

bool SessionManager::AddSession(mgws::context* ctx, Connection* c) {
	try {
		SessionPtr session = m_factory(ctx, *c);
		session->Send({ {"type", "SessionID"}, {"id", session->GetId() } });
	}
	catch (std::exception& e) {
		TRACE("Exception: " << e.what());
		return false;
	}
	return true;
}
void SessionManager::DeleteSession(Session* session) {
	if (!session) {
		TRACE(__FUNCTION__ << "Early return");
		return;
	}

	TRACE(__FUNCTION__ << "(" << session->GetId() << ")");
	delete session;

	UpdateSessionsList();
}

void SessionManager::UpdateSession(const SessionID_t id, const std::string& userName) {
	auto session = GetSessionById(std::to_string(id));

	session->SetUserName(userName);
}

SessionManager::SessionPtr SessionManager::GetSessionById(const SessionID_t sessId) {
	SessionPtr session{ nullptr };

	Iterate([&](Session* sess) {
		if (sess->GetId() == sessId) {
			session = sess;
		}
	});
	return session;
}

SessionManager::SessionPtr SessionManager::GetSessionById(const std::string& sessId)
{
	return GetSessionById(static_cast<SessionID_t>(std::stoi(sessId)));
}

void SessionManager::Iterate(SessionCallback_fn fn) {
	for (mg_connection* c = m_mgr.conns; c != nullptr; c = c->next) {
		if (!c->is_websocket)
			return;

		if (c->fn_data == nullptr) {
			TRACE(__FUNCTION__ << "c->fn_data is null");
			return;
		}

		auto context = (mgws::context*)c->fn_data;
		if (context->user_data == nullptr) {
			TRACE(__FUNCTION__ << "user_data is null");
			return;
		}

		Session* session = (Session*)context->user_data;
		fn(session);
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
