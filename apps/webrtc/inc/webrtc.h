#ifndef WEBRTC_H
#define WEBRTC_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace webrtc {
	namespace localId {
		struct Event {
			std::string type;
			uint32_t sessionId;
			std::string userName;
			std::string localId;
		};

		void to_json(json& j, const Event& e);
		void from_json(const json& j, Event& e);
	}

	namespace sessionsChanged {
		struct Session {
			uint32_t sessionId;
			std::string userName;
			std::string localId;
		};
		struct Message {
			std::string type{};
			std::vector<Session> sessions;
		};

		void to_json(json& j, const Session& s);
		void from_json(const json& j, Session& s);

		void to_json(json& j, const Message& m);
		void from_json(const json& j, Message& m);
	}

	namespace sessionDescription {
		struct SessionDescription {
			std::string type;
			std::string sdp;
		};
		void to_json(json& j, const SessionDescription& e);
		void from_json(const json& j, SessionDescription& e);
	}

	namespace call {
		struct Call {
			std::string type;
			uint32_t sessionId;
			std::string callerUserName;
			std::string targetId;
			std::string callingId;
			sessionDescription::SessionDescription session;
		};
		void to_json(json& j, const Call& cr);
		void from_json(const json& j, Call& cr);
	}
}

#endif
