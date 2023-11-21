#include <iostream>
#include <fstream>
#include <sstream>

#include "webrtc.h"
#include "mongoose.h"

namespace {
    std::string root_dir("../www/mgws/");
    std::string cert_pem_file("localhost.crt");
    std::string key_pem_file("localhost.key");
    std::string cert;
    std::string key;
};

// Mongoose event handler function, gets called by the mg_mgr_poll()
static void fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data) {
    if (ev == MG_EV_ACCEPT && fn_data != NULL) {
        struct mg_tls_opts opts;
        
        memset(&opts, 0, sizeof(opts));

        opts.cert = mg_str(cert.c_str());
        opts.key = mg_str(key.c_str());

        mg_tls_init(c, &opts);
    }
    else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* hm = (struct mg_http_message*)ev_data;

        if (mg_http_match_uri(hm, "/websock")) {
            mg_ws_upgrade(c, hm, NULL);
        }
        // If the requested URI is "/api/hi", send a simple JSON response back
        else if (mg_http_match_uri(hm, "/api/hi")) {
            mg_http_reply(c, 200, "", "{%m:%m,%m:%m}\n",  // See mg_snprintf doc
                MG_ESC("uri"), mg_print_esc, hm->uri.len, hm->uri.ptr,
                MG_ESC("body"), mg_print_esc, hm->body.len, hm->body.ptr);
        }
        else {
            struct mg_http_serve_opts opts;
            memset(&opts, 0, sizeof(opts));
            opts.root_dir = root_dir.c_str();
            mg_http_serve_dir(c, hm, &opts);
        }
    }
    else if (ev == MG_EV_WS_MSG) {
        struct mg_ws_message* wm = (struct mg_ws_message*)ev_data;
        mg_ws_send(c, wm->data.ptr, wm->data.len, WEBSOCKET_OP_TEXT);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " root_dir [cert_file_pem key_file_pem]" << std::endl;
        return -1;
    }

    root_dir = argv[1];

    if (argc > 2) {
        cert_pem_file = argv[2];
        key_pem_file = argv[3];
    }

    std::stringstream buffer;
        
    std::ifstream cert_ifs(cert_pem_file);
    buffer << cert_ifs.rdbuf();
    cert = buffer.str();

    buffer = std::stringstream();
    std::ifstream key_ifs(key_pem_file);
    buffer << key_ifs.rdbuf();
    key = buffer.str();

    std::cout << "MGWS: serving from " << root_dir << ", cert: " << cert_pem_file << ", key: " << key_pem_file << std::endl;
    struct mg_mgr mgr;

    mg_log_set(MG_LL_DEBUG);

    mg_mgr_init(&mgr);

    mg_http_listen(&mgr, "http://0.0.0.0:8000", fn, NULL);
    mg_http_listen(&mgr, "https://0.0.0.0:8443", fn, (void*)1);
	
    while(true)
        mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);

	return 0;
}
