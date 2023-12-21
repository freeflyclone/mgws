#ifndef MGWS_H
#define MGWS_H

#include <string>
#include <iostream>

extern "C" {
	#include <mongoose.h>
	#include <sqlite3.h>
	#include <sqlite3ext.h>
}

class mgws {
public:
	struct context {
		mgws& _mgws;
		struct mg_connection* c;
	} m_context;

	mgws(
		const std::string& root, 
		const std::string& cert_name,
		const std::string& key_name);

	void read_pem(const std::string&, std::string&);

	void infinite_loop();


protected:
	struct mg_mgr m_mgr;
	mg_tls_opts m_tls_opts;
	mg_http_serve_opts m_http_serve_opts;
	std::string m_root_dir;
	std::string m_cert;
	std::string m_key;

private:
	static void _fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
	virtual void fn(struct mg_connection* c, int ev, void* ev_data, context*);
};

#ifdef _WIN32
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif __linux__
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


#define TRACE(...) {std::cerr << __FILENAME__ << ":" << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

#endif