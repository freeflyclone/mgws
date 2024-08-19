#include "websock.h"
#include "sessionmgr.h"

#define MQ_TRACE TRACE

WebSock::WebSock(mgws::context* ctx, Connection& c)
	: Session(ctx, c),
	m_sessions((SessionManager*)ctx->_mgws),
	m_lastHeartbeat(mg_millis())
{
	MQ_TRACE(__FUNCTION__);
	m_md["RegisterSession"] = std::bind(&WebSock::OnRegisterSession, this, std::placeholders::_1);
}

WebSock::~WebSock() {
	MQ_TRACE(__FUNCTION__);
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

	MQ_TRACE(__FUNCTION__ << "(): id: " << sessionId << ", appVersion: " << appVersion << ", userName: " << userName);
}

void WebSock::OnLocalIdEvent(json& j){
	(void)j;
	MQ_TRACE(__FUNCTION__);
}

void WebSock::OnHeartbeat(json& j){
	(void)j;
	MQ_TRACE(__FUNCTION__);
}
