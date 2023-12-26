#include <map>
#include <string>

#include "sessionmgr.h"
#include "peer.h"

Peer::Peer(mgws::context* ctx, Connection& c)
	: Session(ctx, c),
	m_sessions((SessionManager*)ctx->_mgws),
	m_lastHeartbeat(mg_millis())
{
	using namespace std::placeholders;

	//TRACE(__FUNCTION__ << "() id: " << m_id);

	// The set of WebRTC message "type" JSONs we respond to. 
	m_pmd["RegisterSession"] = std::bind(&Peer::OnRegisterSession, this, _1);
	m_pmd["LocalIdEvent"]    = std::bind(&Peer::OnLocalIdEvent,    this, _1);
	m_pmd["ICECandidate"]    = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Call"]            = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Answer"]          = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Hangup"]          = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Heartbeat"]       = std::bind(&Peer::OnHeartbeat, 	   this, _1);
}

Peer::~Peer() {
	//TRACE(__FUNCTION__);
}

void Peer::OnMessage(Message* msg) {
	try {
		auto j = json::parse(std::string(msg->data.ptr, msg->data.len));
		HandleMessage(j);
	}
	catch (std::exception& e) {
		TRACE("OnMessage exception: " << e.what());
	}
}

void Peer::HandleMessage(json& j)
{
	try {
		m_pmd[j["type"]](j);
	}
	catch (std::exception& e) {
		TRACE("Error while handling " << j["type"] << ": " << e.what());
	}
}

void Peer::OnRegisterSession(json& j) 
{
	auto sessionId = j["sessionId"];
	auto appVersion = j["appVersion"];
	auto userName = j["userName"];

	TRACE(__FUNCTION__ << "(): id: " << sessionId << ", appVersion: " << appVersion << ", userName: " << userName);
}

void Peer::OnLocalIdEvent(json& j)
{
	try {
		auto sessionId = j["sessionID"];
		auto userName = j["userName"];

		if (sessionId != GetId()) {
			TRACE("Oops: received sessionID doesn't match m_id: " << sessionId << " vs " << GetId());
			return;
		}

		m_sessions->UpdateSession(GetId(), userName);
		Send({ {"type", "LocalIdChanged"} });

		m_sessions->UpdateSessionsList();
	}
	catch (std::exception& e) {
		TRACE("Error while handling LocalIdEvent: " << e.what());
	}
}

void Peer::OnForwardMessage(json& j)
{
	try {
		auto type = j["type"];
		std::string targetId(j["targetId"]);

		auto session = m_sessions->GetSessionById(targetId);
		if (session)
			session->Send(j);

		// don't log this
		if (j["type"] == "ICECandidate")
			return;

		TRACE(__FUNCTION__ << "(): type: " << j["type"] << ", from: " << j["userName"] << ", to: " << j["targetId"]);
	}
	catch (std::exception& e) {
		TRACE("Error while handling ForwardMessage: " << e.what());
	}
}

void Peer::OnHeartbeat(json& j) {
	if (j["sessionId"] != GetId()) {
		TRACE(__FUNCTION__ << "() Oops, id mismatch, m_id: " << GetId() << ", message: " << j.dump());
	}
	Send(j);
	m_lastHeartbeat = mg_millis();
}
