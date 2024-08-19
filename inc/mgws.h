#ifndef MGWS_H
#define MGWS_H

#include <string>
#include <iostream>
#include <functional>
#include <list>

extern "C" {
	#include <mongoose.h>
	#include <sqlite3.h>
	#include <sqlite3ext.h>
}

class mgws {
public:
	typedef std::vector<std::string> listen_list;
	static const int m_poll_interval_ms{ 500 };
	struct context {
		// MUST be a ptr for polymorphism to work!
		mgws* _mgws;
		void* user_data;
	} m_context;
	typedef std::function<void(context*)> timer_fn;
	mgws(
		const std::string& root,
		const std::string& addr_port, 
		const std::string& cert_name,
		const std::string& key_name);

	void read_pem(const std::string&, std::string&);

	void infinite_loop();

	virtual void fn(struct mg_connection* c, int ev, void* ev_data, context*);
	virtual void timer_event(int64_t);

protected:
	struct mg_mgr m_mgr;
	mg_tls_opts m_tls_opts;
	mg_http_serve_opts m_http_serve_opts;
	std::string m_root_dir;
	std::string m_addr_port;
	std::string m_cert;
	std::string m_key;
	std::list<timer_fn> m_timers;

	static void _fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
	static void _timer_event(void*);
};

#ifdef _WIN32
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif __linux__
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


#define TRACE(...) {std::cerr << __FILENAME__ << ":" << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

#endif
