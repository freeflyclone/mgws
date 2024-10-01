#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "sessionmgr.h"
#include "mqtt.h"
#include "websock.h"

void usage() {
    puts(
        "dashboard "
        "[rootDir [<cert PEM file> <key PEM file>]] "
    );
}

int main(int argc, char *argv[]) {
    char _defaultRootDir[] = "../www/dashboard";
    std::string cert_pem_file("localhost.crt");
    std::string key_pem_file("localhost.key");

    char* rootDir = _defaultRootDir; 

    if (argc > 1)
        rootDir = argv[1];

    if (argc > 2) {
        cert_pem_file = argv[2];
        key_pem_file = argv[3];
    }

    TRACE(__FUNCTION__);

    // Note: do NOT specify the same port twice!
    // (even when http: vs https: in URL)
    mgws::listen_list listeners;
    listeners.push_back("http://0.0.0.0:8000");
    
    std::string root(rootDir);

    auto sm = new SessionManager(root, listeners, cert_pem_file, key_pem_file);

    sm->SetFactory([] (mgws::context* ctx, Connection& c) {
        return new WebSock(ctx, c);
    });

    sm->infinite_loop();

    TRACE(__FUNCTION__ << "() done.");
}
