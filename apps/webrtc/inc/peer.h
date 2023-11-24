#ifndef PEER_H
#define PEER_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Session;

class Peer {
public:
	typedef std::map<std::string, std::function<void(json& j)>> PeerMessageDispatch;

	explicit Peer(Session* sess);
	~Peer() {};

	void HandleMessage(json&);

	void OnRegisterSession(json&);
	void OnLocalIdEvent(json&);
	void OnOffer(json&);
	void OnCall(json&);
	void OnAnswer(json&);
	void OnIceCandidate(json&);

private:
	Session* m_session;
	PeerMessageDispatch m_pmd;
};

#endif