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
	namespace callRemote {
		void to_json(json& j, const CallRemote& cr) {
		}

		void from_json(const json& j, CallRemote& cr) {
			try {
				j.at("type").get_to(cr.type);
				j.at("sessionId").get_to(cr.sessionId);
				j.at("remoteId").get_to(cr.remoteId);
				j.at("userName").get_to(cr.userName);
				j.at("session").get_to(cr.session);
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
	m_pmd["CallRemote"] = std::bind(&Peer::OnCallRemote, this, _1);
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

void Peer::OnOffer(json& j)
{
	TRACE(__FUNCTION__ << ": " << j["type"]);

	//TRACE(__FUNCTION__ << ": " << j.dump(4, '-'));
}

void Peer::OnCallRemote(json& j) 
{
	auto callRemote = j.template get<webrtc::callRemote::CallRemote>();
	auto remoteId = callRemote.remoteId;
	std::string::size_type n;
	if ((n = remoteId.find("_00")) == -1) {
		TRACE("Didn't find '_00'");
		return;
	}
	auto id = std::stod(remoteId.substr(n + 3));
	auto session = g_sessions.GetSessionById(id).get();

	session->Send(callRemote.session);

	TRACE(__FUNCTION__ << ": " << callRemote.userName << ", remote ID: " << callRemote.remoteId);
	//TRACE(__FUNCTION__ << ": " << j.dump(4, '-'));
}
