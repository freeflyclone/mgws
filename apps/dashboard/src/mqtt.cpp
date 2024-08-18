#include "mqtt.h"
#include "sessionmgr.h"

#define MQ_TRACE TRACE

Mqtt::Mqtt(mgws::context* ctx, Connection& c)
	: Session(ctx, c),
	m_sessions((SessionManager*)ctx->_mgws)
{
	MQ_TRACE(__FUNCTION__ << "() id: " << GetId());
}

Mqtt::~Mqtt() {
	MQ_TRACE(__FUNCTION__ << "() id: " << GetId());
}

void Mqtt::OnMessage(MqttMessage* msg) {
	try {
		MQ_TRACE(__FUNCTION__);
	}
	catch (std::exception& e) {
		MQ_TRACE("OnMessage exception: " << e.what());
	}
}

void Mqtt::OnRegisterSession(json&){
		MQ_TRACE(__FUNCTION__);
}

void Mqtt::OnLocalIdEvent(json&){
		MQ_TRACE(__FUNCTION__);
}

void Mqtt::OnHeartbeat(json&){
		MQ_TRACE(__FUNCTION__);
}
