#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include <mutex>

#include "mgws.h"
#include "session.h"

class SessionManager : public mgws {
public:
	typedef Session* SessionPtr;
	typedef std::function<SessionPtr(mgws::context*, Connection&)> SessionFactory_t;
	typedef std::function<void(Session*)> SessionCallback_fn;

	SessionManager(
		const std::string& root,
		const std::string& cert_name,
		const std::string& key_name);

	~SessionManager() {};

	void SetFactory(SessionFactory_t);

	bool AddSession(mgws::context *, Connection*);
	void DeleteSession(Session*);

	void UpdateSession(const uint32_t sessionId, const std::string&);
	void UpdateSessionsList();

	SessionPtr GetSessionById(const SessionID_t);
	SessionPtr GetSessionById(const std::string& );

	void Iterate(SessionCallback_fn fn);

private:
	virtual void fn(struct mg_connection* c, int ev, void* ev_data, context*) override;
	virtual void timer_event(int64_t) override;

	SessionFactory_t m_factory;
};

#endif
