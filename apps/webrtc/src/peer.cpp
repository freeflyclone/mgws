#include <map>

#include "mgws.h"
#include "peer.h"
#include "session.h"
#include "webrtc.h"

namespace webrtc {
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
}
Peer::Peer(Session* sess) 
  : 
	m_session(sess)
{
	m_pmd["RegisterSession"] = std::bind(&Peer::OnRegisterSession, this, std::placeholders::_1);
	m_pmd["LocalIdEvent"] = std::bind(&Peer::OnLocalIdEvent, this, std::placeholders::_1);
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
	TRACE(__FUNCTION__);
}

void Peer::OnLocalIdEvent(json& j)
{
	TRACE(__FUNCTION__);
	webrtc::sessionsChanged::Message msg;

	msg.type = "SessionsChanged";
}
