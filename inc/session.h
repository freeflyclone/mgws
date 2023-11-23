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
#include "peer.h"

using json = nlohmann::json;

extern "C" {
#include "mongoose.h"
}

typedef uint32_t SessionID_t;
typedef struct mg_connection Connection;
typedef struct mg_ws_message Message;

class Session {
public:
	explicit Session(uint32_t it, Connection& c);
	~Session();

	SessionID_t getId() { return m_id; };
	void SetUserName(const std::string& name) { m_userName = name; }
	void SetLocalId(const std::string& localId) { m_localId = localId; }
	void Send(const json&);
	void OnMessage(Message*);

private:
	SessionID_t m_id;
	Connection& m_connection;
	Peer m_peer;

	std::string m_localId;
	std::string m_userName;
};

#endif // SESSION_H