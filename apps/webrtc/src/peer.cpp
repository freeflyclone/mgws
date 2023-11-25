#include <map>
#include <string>

#include "mgws.h"
#include "peer.h"
#include "session.h"
#include "sessionmgr.h"
#include "webrtc.h"

namespace webrtc {
	namespace localId {
		void to_json(json& j, const Event& e) {
			try {
				j = json{
					{"type", e.type},
					{"sessionID", e.sessionId},
					{"userName", e.userName},
					{"localId", e.localId}
				};
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}
		void from_json(const json& j, Event& e) {
			try {
				j.at("type").get_to(e.type);
				j.at("sessionID").get_to(e.sessionId);
				j.at("userName").get_to(e.userName);
				j.at("localId").get_to(e.localId);
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}
	}
	namespace sessionsChanged {
		void to_json(json& j, const Session& s) {
			try {
				j = json{
					{ "sessionID", s.sessionId },
					{ "userName", s.userName},
					{ "localId", s.localId}
				};
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}

		void from_json(json& j, Session& s) {
			try {
				j.at("sessionID").get_to(s.sessionId);
				j.at("userName").get_to(s.userName);
				j.at("localId").get_to(s.localId);
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}

		void to_json(json& j, const Message& m) {
			try {
				j = json{
					{"type", m.type},
					{"sessions", m.sessions}
				};
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}

		void from_json(const json& j, Message& m) {
			try {
				j.at("type").get_to(m.type);

				for (auto js : j.at("sessions")) {
					Session s{ 0, "", "" };

					if (js.contains("sessionID"))
						js.at("sessionID").get_to<uint32_t>(s.sessionId);

					if (js.contains("userName"))
						js.at("userName").get_to(s.userName);

					if (js.contains("localId"))
						js.at("localId").get_to(s.localId);

					m.sessions.push_back(s);
				}
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}
	}
	namespace sessionDescription {
		void to_json(json& j, const SessionDescription& sd) {
			try {
				j = json{
					{"type", sd.type},
					{"sdp", sd.sdp}
				};
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}
		void from_json(const json& j, SessionDescription& sd) {
			try {
				j.at("type").get_to(sd.type);
				j.at("sdp").get_to(sd.sdp);
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}
	}
	namespace call {
		void to_json(json& j, const Call& call) {
			(void)j;
			(void)call;
		}

		void from_json(const json& j, Call& call) {
			try {
				j.at("type").get_to(call.type);
				j.at("sessionId").get_to(call.sessionId);
				j.at("callerUserName").get_to(call.callerUserName);
				j.at("targetId").get_to(call.targetId);
				j.at("callingId").get_to(call.callingId);
				j.at("session").get_to(call.session);
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}
	}
	namespace answer {
		void to_json(json& j, const Answer& answer) {
			(void)j;
			(void)answer;
		}

		void from_json(const json& j, Answer& answer) {
			try {
				j.at("type").get_to(answer.type);
				j.at("sessionId").get_to(answer.sessionId);
				j.at("answeringUserName").get_to(answer.answeringUserName);
				j.at("targetId").get_to(answer.targetId);
				j.at("answeringId").get_to(answer.answeringId);
				j.at("session").get_to(answer.session);
			}
			catch (const json::exception& e) {
				TRACE(__FUNCTION__ << e.what());
			}
		}
	}
}


Peer::Peer(Session* sess)
  : 
	m_session(sess)
{
	using namespace std::placeholders;

	m_pmd["RegisterSession"] = std::bind(&Peer::OnRegisterSession, this, _1);
	m_pmd["LocalIdEvent"] = std::bind(&Peer::OnLocalIdEvent, this, _1);
	m_pmd["offer"] = std::bind(&Peer::OnOffer, this, _1);
	m_pmd["Call"] = std::bind(&Peer::OnCall, this, _1);
	m_pmd["Answer"] = std::bind(&Peer::OnAnswer, this, _1);
	m_pmd["ICECandidate"] = std::bind(&Peer::OnIceCandidate, this, _1);
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
		TRACE(__FUNCTION__ << j);

		auto localIdEvent = j.template get<webrtc::localId::Event>();

		if (localIdEvent.sessionId != m_session->getId()) {
			TRACE("Oops: received sessionID doesn't match m_id: " << localIdEvent.sessionId << " vs " << m_session->getId());
			return;
		}

		g_sessions.UpdateSession(m_session->getId(), localIdEvent.userName, localIdEvent.localId);
		m_session->Send({ {"type", "LocalIdChanged"} });
		g_sessions.UpdateSessionsList();
	}
	catch (std::exception& e) {
		TRACE("Error while handling " << j["type"] << ": " << e.what());
	}
}

void Peer::OnOffer(json& j)
{
	TRACE(__FUNCTION__ << ": " << j["type"]);

	//TRACE(__FUNCTION__ << ": " << j.dump(4, '-'));
}

void Peer::OnCall(json& j)
{
	try {
		auto call = j.template get<webrtc::call::Call>();
		auto session = g_sessions.GetSessionByLocalId(call.targetId);

		TRACE(__FUNCTION__ << ": " << call.callerUserName << ", target ID: " << call.targetId);
		//TRACE(__FUNCTION__ << ": " << j.dump(4, '-'));

		if (session)
			session->Send(j);
	}
	catch (std::exception& e) {
		TRACE("Error while handling " << j["type"] << ": " << e.what());
	}
}

void Peer::OnAnswer(json& j)
{
	try {
		auto answer = j.template get<webrtc::answer::Answer>();
		auto session = g_sessions.GetSessionByLocalId(answer.targetId);

		TRACE(__FUNCTION__ << ": " << answer.answeringUserName << ", target ID: " << answer.targetId);
		//TRACE(__FUNCTION__ << ": " << j.dump(4, '-'));

		if (session)
			session->Send(j);
	}
	catch (std::exception& e) {
		TRACE("Error while handling " << j["type"] << ": " << e.what());
	}
}

void Peer::OnIceCandidate(json& j)
{
	auto candidate = j["candidate"]["candidate"];
	auto targetId = j["targetId"];

	auto session = g_sessions.GetSessionByLocalId(targetId);

	TRACE(__FUNCTION__ << "(): id: " << m_session->getId() << ", " << targetId << ", " << candidate.dump(4, '+'));

	if (session) {
		session->Send(j);
	}
}
