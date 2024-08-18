#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "sessionmgr.h"

void usage() {
    puts(
        "dashboard "
        "[rootDir] "
    );
}

int main(int argc, char *argv[]) {
    char _defaultRootDir[] = "../www/dashboard";
    char* rootDir = _defaultRootDir; 

    if (argc > 1)
        rootDir = argv[1];

    TRACE(__FUNCTION__);

    std::string root(rootDir);
    std::string addr_port("https://0.0.0.0:443");
    std::string cert_pem_file("/etc/letsencrypt/live/lumenicious.com/fullchain.pem");
    std::string key_pem_file("/etc/letsencrypt/live/lumenicious.com/privkey.pem");

    auto sm = new SessionManager(root, addr_port, cert_pem_file, key_pem_file);

    sm->infinite_loop();

    TRACE(__FUNCTION__ << "() done.");
}
