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

	explicit Dashboard(mgws::context*, Connection&);
	virtual ~Dashboard();

	// Session management API from browser JS code
	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);
	void OnHeartbeat(json&);

private:
	SessionManager* m_sessMgr;
	int64_t m_lastHeartbeat;
};

#endif