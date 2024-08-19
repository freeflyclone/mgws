#ifndef WEBSOCK_H
#define WEBSOCK_H

#include <nlohmann/json.hpp>
#include "mgws.h"
#include "session.h"

using namespace nlohmann::literals;
using json = nlohmann::json;

class SessionManager;

class WebSock : public Session {
public:
	explicit WebSock(mgws::context*, Connection&);
	virtual ~WebSock();

	void OnMessage(Message*);

	// Session management API from browser JS code
	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);
	void OnHeartbeat(json&);

private:
	SessionManager* m_sessions;
};

#endif