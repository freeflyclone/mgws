#include "webrtc.h"
#include "mongoose.h"

// Mongoose event handler function, gets called by the mg_mgr_poll()
static void fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* hm = (struct mg_http_message*)ev_data;

        // If the requested URI is "/api/hi", send a simple JSON response back
        if (mg_http_match_uri(hm, "/api/hi")) {
            mg_http_reply(c, 200, "", "{%m:%m,%m:%m}\n",  // See mg_snprintf doc
                MG_ESC("uri"), mg_print_esc, hm->uri.len, hm->uri.ptr,
                MG_ESC("body"), mg_print_esc, hm->body.len, hm->body.ptr);
        }
        else {
            struct mg_http_serve_opts opts { 0 };
            opts.root_dir = "../www/mgws";
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}


int main(int argc, char* argv[])
{
	struct mg_mgr mgr;

	mg_mgr_init(&mgr);

    mg_http_listen(&mgr, "http://0.0.0.0:8000", fn, NULL);
	
    while(true)
        mg_mgr_poll(&mgr, 1000);

	return 0;
}
