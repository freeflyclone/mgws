#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include <mutex>

#include "mgws.h"
#include "session.h"

class SessionManager : public mgws {
public:
	typedef std::function<void(Session*)> SessionCallback_fn;
	typedef std::shared_ptr<Session> SessionPtr;
	typedef std::map<SessionID_t, SessionPtr> SessionsList;

	SessionManager(
		const std::string& root,
		const std::string& cert_name,
		const std::string& key_name);

	~SessionManager() {};

	bool AddSession(SessionPtr);
	void DeleteSession(Session*);
	void UpdateSession(const uint32_t sessionId, const std::string&);
	SessionPtr GetSessionById(const std::string& );

	void Iterate(SessionCallback_fn fn);

	void UpdateSessionsList();

private:
	virtual void fn(struct mg_connection* c, int ev, void* ev_data, context*) override;

	std::mutex m_idMutex;
	SessionsList m_sessions;
	uint32_t m_nextId;
};

#endif
