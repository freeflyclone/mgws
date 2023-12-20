#ifndef MGWS_H
#define MGWS_H

#include <iostream>
extern "C" {
	#include <mongoose.h>
	#include <sqlite3.h>
	#include <sqlite3ext.h>
}

class mgws {
public:
	mgws();
	~mgws();

	void infiniteLoop();

protected:
	struct mg_mgr m_mgr;
};

#ifdef _WIN32
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif __linux__
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


#define TRACE(...) {std::cerr << __FILENAME__ << ":" << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

#endif