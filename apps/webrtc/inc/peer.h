#ifndef PEER_H
#define PEER_H

#include <nlohmann/json.hpp>

using namespace nlohmann::literals;
using json = nlohmann::json;

class Session;

class Peer {
public:
	typedef std::map<std::string, std::function<void(json& j)>> PeerMessageDispatch;

	explicit Peer(Session* sess);
	~Peer() {};

	void HandleMessage(json&);

	// Usually we just want to send msgs to another user
	void OnForwardMessage(json&);

	void OnRegisterSession(json&);

	void OnLocalIdEvent(json&);

private:
	Session* m_session;
	PeerMessageDispatch m_pmd;
	size_t m_user_connection_id;
};

#endif