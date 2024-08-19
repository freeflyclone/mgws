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
	static const int64_t m_sessionTimeoutMs = 5000;
	typedef std::map<std::string, std::function<void(json& j)>> WebSockMessageDispatch;

	explicit WebSock(mgws::context*, Connection&);
	virtual ~WebSock();

	void OnMessage(Message*);
	void HandleMessage(json&);

	// Session management API from browser JS code
	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);
	void OnHeartbeat(json&);

private:
	SessionManager* m_sessions;
	WebSockMessageDispatch m_md;
	int64_t m_lastHeartbeat;
};

#endif