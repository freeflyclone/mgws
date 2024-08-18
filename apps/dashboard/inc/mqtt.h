#ifndef MQTT_H
#define MQTT_H

#include <nlohmann/json.hpp>
#include "session.h"

using namespace nlohmann::literals;
using json = nlohmann::json;

class SessionManager;

class Mqtt : public Session {
public:
	explicit Mqtt(mgws::context*, Connection&);
	virtual ~Mqtt();

	void OnMessage(MqttMessage*);

	// Session management API from browser JS code
	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);
	void OnHeartbeat(json&);

private:
	SessionManager* m_sessions;
};

#endif