#include "websock.h"
#include "sessionmgr.h"

#define MQ_TRACE TRACE

WebSock::WebSock(mgws::context* ctx, Connection& c)
	: Session(ctx, c),
	m_sessions((SessionManager*)ctx->_mgws),
	m_lastHeartbeat(mg_millis())
{
	MQ_TRACE(__FUNCTION__);
}

WebSock::~WebSock() {
	MQ_TRACE(__FUNCTION__);
}

void WebSock::OnMessage(Message* msg) {
    (void)msg;
	try {
		MQ_TRACE(__FUNCTION__);
	}
	catch (std::exception& e) {
		MQ_TRACE("OnMessage exception: " << e.what());
	}
}

void WebSock::OnRegisterSession(json&){
		MQ_TRACE(__FUNCTION__);
}

void WebSock::OnLocalIdEvent(json&){
		MQ_TRACE(__FUNCTION__);
}

void WebSock::OnHeartbeat(json&){
		MQ_TRACE(__FUNCTION__);
}
