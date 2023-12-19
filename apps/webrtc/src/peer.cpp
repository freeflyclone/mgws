#include <map>
#include <string>

#include "mgws.h"
#include "peer.h"
#include "session.h"
#include "sessionmgr.h"

Peer::Peer(Connection& c)
	: Session(c)
{
	using namespace std::placeholders;

	m_pmd["RegisterSession"] = std::bind(&Peer::OnRegisterSession, this, _1);
	m_pmd["LocalIdEvent"]    = std::bind(&Peer::OnLocalIdEvent,    this, _1);
	m_pmd["ICECandidate"]    = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Call"]            = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Answer"]          = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Hangup"]          = std::bind(&Peer::OnForwardMessage,  this, _1);
}

void Peer::OnMessage(Message* msg) {
	try {
		std::string msgString(msg->data.ptr, msg->data.len);
		TRACE("OnMessage: " << msgString);

		HandleMessage(json::parse(msgString));
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

		g_sessions.UpdateSession(GetId(), userName);
		Send({ {"type", "LocalIdChanged"} });

		g_sessions.UpdateSessionsList();
	}
	catch (std::exception& e) {
		TRACE("Error while handling LocalIdEvent: " << e.what());
	}
}

void Peer::OnForwardMessage(json& j)
{
	try {
		auto type = j["type"];
		auto targetId = j["targetId"];
		//TRACE(__FUNCTION__ << "type: " << type << ", targetId: " << targetId);

		auto session = g_sessions.GetSessionById(targetId);
		if (session)
			session->Send(j);

		// don't log this
		if (j["type"] == "ICECandidate")
			return;

		TRACE(__FUNCTION__ << "type: " << j["type"] << ", from: " << j["userName"] << ", to: " << j["targetId"]);
	}
	catch (std::exception& e) {
		TRACE("Error while handling ForwardMessage: " << e.what());
	}
}
