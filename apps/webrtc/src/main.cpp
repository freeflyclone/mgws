#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "webrtc.h"

#include "sessionmgr.h"
#include "peer.h"

int main(int argc, char* argv[]) 
{
    std::string root("../www/mgws/");
    std::string cert_pem_file("localhost.crt");
    std::string key_pem_file("localhost.key");

    root = argv[1];

    if (argc < 2) {
        printf("usage: %s <root_folder> [ <cert PEM file> <key PEM file> ]\n", argv[0]);
        return 1;
    }

    if (argc > 2) {
        cert_pem_file = argv[2];
        key_pem_file = argv[3];
    }

    auto sm = new SessionManager(root.c_str(), cert_pem_file.c_str(), key_pem_file.c_str());

    sm->SetFactory([] (mgws::context* ctx, Connection& c) {
        return new Peer(ctx, c);
    });

    sm->infinite_loop();
}
