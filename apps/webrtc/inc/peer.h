/**
* Peer is a WebRTC signaling server, with JSON call message
* packets.
*
* It implements a function dispatch table based on JSON "type" field
* present in received JSON strings.
* 
* PeerDispatchMap ("m_pmd") declares the table layout.
* 
* This is as minimial as possible, to aid in microcontroller deployment
* in the future.
*/
#ifndef PEER_H
#define PEER_H

#include <nlohmann/json.hpp>
#include <session.h>

using namespace nlohmann::literals;
using json = nlohmann::json;

class Peer : public Session {
public:
	static const int64_t m_sessionTimeoutMs = 5000;
	typedef std::map<std::string, std::function<void(json& j)>> PeerMessageDispatch;

	explicit Peer(mgws::context*, Connection&);
	virtual ~Peer();

	void OnMessage(Message*) override;
	void HandleMessage(json&);

	// Usually we just want to send msgs to another user
	void OnForwardMessage(json&);

	// Session management API from browser JS code
	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);
	void OnHeartbeat(json&);

private:
	PeerMessageDispatch m_pmd;
	SessionManager* m_sessions;
	int64_t m_lastHeartbeat;
};

#endif