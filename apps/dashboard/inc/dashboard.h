#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <nlohmann/json.hpp>
#include "session.h"

using namespace nlohmann::literals;
using json = nlohmann::json;

class SessionManager;

class Dashboard : public Session {
public:
	static const int64_t m_sessionTimeoutMs = 5000;
	typedef std::map<std::string, std::function<void(json& j)>> DashboardMessageDispatch;

	explicit Dashboard(mgws::context*, Connection&);
	virtual ~Dashboard();

	void OnMessage(Message*) override;
	void HandleMessage(json&);

	// Usually we just want to send msgs to another user
	void OnForwardMessage(json&);

	// Session management API from browser JS code
	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);
	void OnHeartbeat(json&);

private:
	DashboardMessageDispatch m_md;
	SessionManager* m_sessions;
	int64_t m_lastHeartbeat;
};

#endif