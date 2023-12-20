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
	mgws(
		const std::string& root, 
		const std::string& cert_name,
		const std::string& key_name);

	const char* readPEM(char* name);

	void infiniteLoop();

	static void fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data);

protected:
	struct mg_mgr m_mgr;
	mg_tls_opts m_opts;
	std::string m_root_dir;
	std::string m_cert;
	std::string m_key;
};

#ifdef _WIN32
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif __linux__
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


#define TRACE(...) {std::cerr << __FILENAME__ << ":" << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

#endif