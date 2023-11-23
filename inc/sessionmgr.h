#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include "session.h"

class SessionManager {
public:
	typedef std::shared_ptr<Session> SessionPtr;
	typedef std::map<SessionID_t, SessionPtr> SessionsList;

	explicit SessionManager() {};
	~SessionManager() {};


	SessionPtr NewSession(Connection&);
	void DeleteSession(Session*);

private:
	SessionsList m_sessions;
};

extern SessionManager g_sessions;

#endif