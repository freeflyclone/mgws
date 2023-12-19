#ifndef PEER_H
#define PEER_H

#include <nlohmann/json.hpp>
#include <session.h>

using namespace nlohmann::literals;
using json = nlohmann::json;

class Peer : public Session {
public:
	typedef std::map<std::string, std::function<void(json& j)>> PeerMessageDispatch;

	explicit Peer(Connection&);
	~Peer() {};

	void OnMessage(Message*) override;
	void HandleMessage(json&);

	// Usually we just want to send msgs to another user
	void OnForwardMessage(json&);

	// Session management API from browser JS code
	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);

private:
	PeerMessageDispatch m_pmd;
};

#endif