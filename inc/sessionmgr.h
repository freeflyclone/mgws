#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include "session.h"
#include "webrtc.h"

class SessionManager {
public:
	typedef std::shared_ptr<Session> SessionPtr;
	typedef std::map<SessionID_t, SessionPtr> SessionsList;

	explicit SessionManager() {};
	~SessionManager() {};


	SessionPtr NewSession(Connection&);
	void DeleteSession(Session*);
	void UpdateSessions(const uint32_t sessionId, const std::string&, const std::string& );

private:
	SessionsList m_sessions;
};

extern SessionManager g_sessions;

#endif