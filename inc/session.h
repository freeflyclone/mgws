#ifndef SESSION_H
#define SESSION_H
/**
* Session is a Websocket session created when mg_ws_upgrade() is called
* in response to a "/websock" URI request is received by Mongoose.
*/

#include <map>
#include <algorithm>
#include <memory>

extern "C" {
#include "mongoose.h"
}

typedef uint32_t SessionID_t;
typedef struct mg_connection Connection;
typedef struct mg_ws_message Message;

class Session {
public:
	explicit Session(Connection& c);
	~Session();

	SessionID_t getId() { return m_id; };
	void OnMessage(Message*);

private:
	SessionID_t m_id;
	Connection& m_connection;
};

typedef std::shared_ptr<Session> SessionPtr;
typedef std::map<SessionID_t, SessionPtr> SessionsList;

SessionPtr NewSession(Connection&);
void DeleteSession(Session*);

#endif // SESSION_H