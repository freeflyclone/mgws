#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "sessionmgr.h"

int main() {
    TRACE(__FUNCTION__);

    std::string root("../www/dashboard/");
    std::string addr_port("https://0.0.0.0:443");
    std::string cert_pem_file("/etc/letsencrypt/live/ws.lumenicious.com/fullchain.pem");
    std::string key_pem_file("/etc/letsencrypt/live/ws.lumenicious.com/privkey.pem");

    auto sm = new SessionManager(root, addr_port, cert_pem_file, key_pem_file);

    sm->infinite_loop();

    TRACE(__FUNCTION__ << "() done.");
}
