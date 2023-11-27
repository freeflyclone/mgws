#ifndef MGWS_H
#define MGWS_H

#include <iostream>
extern "C" {
	#include <sqlite3.h>
	#include <sqlite3ext.h>
}

#ifdef _WIN32
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif __linux__
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


#define TRACE(...) {std::cerr << __FILENAME__ << ":" << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

#endif