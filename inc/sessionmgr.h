#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include <mutex>
#include "session.h"

class SessionManager {
public:
	typedef std::function<void(Session*)> SessionCallback_fn;
	typedef std::shared_ptr<Session> SessionPtr;
	typedef std::map<SessionID_t, SessionPtr> SessionsList;

	explicit SessionManager() {};
	~SessionManager() {};

	bool AddSession(SessionPtr);
	void DeleteSession(Session*);
	void UpdateSession(const uint32_t sessionId, const std::string&);
	SessionPtr GetSessionById(const std::string& );

	void Iterate(SessionCallback_fn fn);

	void UpdateSessionsList();

private:
	std::mutex m_idMutex;
	SessionsList m_sessions;
};

extern SessionManager g_sessions;

#endif