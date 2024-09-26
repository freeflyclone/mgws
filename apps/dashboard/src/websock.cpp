#include "websock.h"
#include "sessionmgr.h"

#define MQ_TRACE TRACE

WebSock::WebSock(mgws::context* ctx, Connection& c)
	: Session(ctx, c),
	m_sessMgr((SessionManager*)ctx->_mgws),
	m_lastHeartbeat(mg_millis())
{
	//MQ_TRACE(__FUNCTION__);
	m_md["RegisterSession"] = std::bind(&WebSock::OnRegisterSession, this, std::placeholders::_1);
	m_md["GetActiveSessions"] = std::bind(&WebSock::OnGetActiveSessions, this, std::placeholders::_1);
}

WebSock::~WebSock() {
	//MQ_TRACE(__FUNCTION__);
}

void WebSock::OnMessage(Message* msg) {
    (void)msg;
	try {
		auto j = json::parse(std::string(msg->data.ptr, msg->data.len));
		HandleMessage(j);
	}
	catch (std::exception& e) {
		MQ_TRACE("OnMessage exception: " << e.what());
	}
}

void WebSock::HandleMessage(json& j)
{
	try {
		m_md[j["type"]](j);
	}
	catch (std::exception& e) {
		MQ_TRACE("Error while handling " << j["type"] << ": " << e.what());
	}
}

void WebSock::OnRegisterSession(json& j){
	auto sessionId = j["sessionId"];
	auto appVersion = j["appVersion"];
	auto userName = j["userName"];

	// TODO: verify user name in Sqlite3 DB
	SetUserName(userName);

	MQ_TRACE(__FUNCTION__ << "(): sessionId: " << sessionId << ", appVersion: " << appVersion << ", userName: " << userName);
}

void WebSock::OnGetActiveSessions(json& j){
	auto sessionId = j["sessionId"];
	//auto appVersion = j["appVersion"];
	//auto userName = j["userName"];

	auto mg_mgr = m_connection.mgr;
	int counter = 0;
	std::string userNames;

	json response;
	response["type"] = "ActiveSessions";
	response["sessionId"] = GetId();

	for (auto connection = mg_mgr->conns; connection != nullptr; connection = connection->next) {
		if (connection->is_websocket) {

			auto context = (mgws::context*)connection->fn_data;
			if (context->user_data == nullptr) {
				MQ_TRACE(__FUNCTION__ << "user_data is null");
				return;
			}
	
			Session* session = (Session*)context->user_data;
			json activeSession;

			activeSession["sessionId"] = session->GetId();
			activeSession["userName"] = session->GetUserName();
			response["sessions"].push_back(activeSession);

			userNames += session->GetUserName() + ", ";
			counter++;
		}
	}

	Send(response);

	MQ_TRACE(__FUNCTION__ << "(): sessionId: " << sessionId << ", " << counter << " active " << (counter == 1 ? "session" : "sessions"));
}

void WebSock::OnLocalIdEvent(json& j){
	(void)j;
	MQ_TRACE(__FUNCTION__);
}

void WebSock::OnHeartbeat(json& j){
	(void)j;
	MQ_TRACE(__FUNCTION__);
}
