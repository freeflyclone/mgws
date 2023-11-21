#ifndef WEBRTC_H
#define WEBRTC_H

#include <string>
#include <vector>
namespace webrtc {
	namespace localId {
		struct Event {
			std::string type;
			uint32_t sessionId;
			std::string userName;
			std::string localId;
		};
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
	}

	namespace sessionDescription {
		struct SessionDescription {
			std::string type;
			std::string sdp;
		};
	}

	namespace callRemote {
		struct CallRemote {
			std::string type;
			uint32_t sessionId;
			std::string remoteId;
			std::string userName;
			sessionDescription::SessionDescription session;
		};
	}
}

#endif
