#ifndef SESSION_H
#define SESSION_H
/**
* Session is a Websocket session, created when mg_ws_upgrade() is called
* in response to a "/websock" URI request is received by Mongoose.
*/

#include <map>
#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern "C" {
#include "mongoose.h"
}

typedef uint32_t SessionID_t;
typedef struct mg_connection Connection;
typedef struct mg_ws_message Message;

class Session {
public:
	explicit Session(SessionID_t, Connection&);
	explicit Session(Connection&);

	~Session();

	SessionID_t GetId();
	void SetId(SessionID_t id);

	const std::string& GetUserName();
	void SetUserName(const std::string& name);

	virtual void Send(const json&);
	virtual void OnMessage(Message*);

protected:
	SessionID_t m_id;
	Connection& m_connection;

	std::string m_userName;
};

#endif // SESSION_H