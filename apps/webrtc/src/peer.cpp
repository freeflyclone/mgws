#include <map>
#include <string>

#include "mgws.h"
#include "peer.h"
#include "session.h"
#include "sessionmgr.h"
#include "webrtc.h"


Peer::Peer(Session* sess)
  : 
	m_session(sess)
{
	using namespace std::placeholders;

	m_pmd["RegisterSession"] = std::bind(&Peer::OnRegisterSession, this, _1);
	m_pmd["LocalIdEvent"]    = std::bind(&Peer::OnLocalIdEvent,    this, _1);
	m_pmd["Call"]            = std::bind(&Peer::OnCall,            this, _1);
	m_pmd["Answer"]          = std::bind(&Peer::OnAnswer,          this, _1);
	m_pmd["ICECandidate"]    = std::bind(&Peer::OnIceCandidate,    this, _1);
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

void Peer::OnCall(json& j)
{
	try {
		auto userName = j["callerUserName"];
		auto targetId = j["targetId"];
		auto session = g_sessions.GetSessionByLocalId(targetId);

		TRACE(__FUNCTION__ << ": " << userName << ", target ID: " << targetId);

		if (session)
			session->Send(j);
	}
	catch (std::exception& e) {
		TRACE("Error while handling Call: " << e.what());
	}
}

void Peer::OnAnswer(json& j)
{
	try {
		auto userName = j["answeringUserName"];
		auto targetId = j["targetId"];
		auto session = g_sessions.GetSessionByLocalId(targetId);

		TRACE(__FUNCTION__ << ": " << userName << ", target ID: " << targetId);

		if (session)
			session->Send(j);
	}
	catch (std::exception& e) {
		TRACE("Error while handling Answer: " << e.what());
	}
}

void Peer::OnIceCandidate(json& j)
{
	try {
		auto targetId = j["targetId"];
		auto session = g_sessions.GetSessionByLocalId(targetId);

		if (session) {
			session->Send(j);
		}
	}
	catch (std::exception& e) {
		TRACE("Error while handling ICECandidate: " << e.what());
	}
}
