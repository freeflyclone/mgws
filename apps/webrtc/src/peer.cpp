#include <map>
#include <string>

#include "mgws.h"
#include "peer.h"
#include "session.h"
#include "sessionmgr.h"

Peer::Peer(Session* sess) :	m_session(sess)
{
	using namespace std::placeholders;

	m_pmd["RegisterSession"] = std::bind(&Peer::OnRegisterSession, this, _1);
	m_pmd["LocalIdEvent"]    = std::bind(&Peer::OnLocalIdEvent,    this, _1);
	m_pmd["ICECandidate"]    = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Call"]            = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Answer"]          = std::bind(&Peer::OnForwardMessage,  this, _1);
	m_pmd["Hangup"]          = std::bind(&Peer::OnForwardMessage,  this, _1);
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
	(void)j;
	TRACE(__FUNCTION__);
}

void Peer::OnLocalIdEvent(json& j)
{
	try {
		auto sessionId = j["sessionID"];
		auto userName = j["userName"];
		auto localId = j["localId"];

		if (sessionId != m_session->getId()) {
			TRACE("Oops: received sessionID doesn't match m_id: " << sessionId << " vs " << m_session->getId());
			return;
		}

		g_sessions.UpdateSession(m_session->getId(), userName, localId);
		m_session->Send({ {"type", "LocalIdChanged"} });

		g_sessions.UpdateSessionsList();
	}
	catch (std::exception& e) {
		TRACE("Error while handling LocalIdEvent: " << e.what());
	}
}

void Peer::OnForwardMessage(json& j)
{
	try {
		auto session = g_sessions.GetSessionByLocalId(j["targetId"]);

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
