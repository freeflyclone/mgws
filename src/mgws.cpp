#include "mgws.h"

#include <fstream>
#include <sstream>
#include <exception>

mgws::mgws(
	const std::string& root,
	const std::string& cert,
	const std::string& key
)
	: m_mgr{},
	m_tls_opts{},
	m_http_serve_opts{},
	m_root_dir(root),
	m_context{*this, nullptr}
{
	TRACE(root << ", " << cert << ", " << key);

	read_pem(cert, m_cert);
	read_pem(key, m_key);

	m_tls_opts.cert = mg_str(m_cert.c_str());
	m_tls_opts.key = mg_str(m_key.c_str());

	m_http_serve_opts.root_dir = m_root_dir.c_str();

	mg_mgr_init(&m_mgr);

	// Each new mg_connection initially gets our "m_context" for its fn_data
	// Allows for both "this" and "mg_connection".
	// 
	// - "this" is for C mongoose callback static void mgws::_fn to establish
	//   the proper pointer for the per-instance mgws::fn() (note the "_" distinction)
	//
	// - "mg_connection" is for per connection use of mg_connection.fn_data to hold "Session" ptr.
	mg_http_listen(&m_mgr, "http://0.0.0.0:8443", mgws::_fn, &m_context);
}

void mgws::infinite_loop()
{
	while (true)
		mg_mgr_poll(&m_mgr, 1000);
}

void mgws::read_pem(const std::string& name, std::string& data) {
	try {
		std::stringstream buffer;
		std::ifstream ifs(name);
		buffer << ifs.rdbuf();
		data = buffer.str();
	}
	catch (std::exception& e) {
		TRACE(__FUNCTION__ << "() exeption: " << e.what());
	}
}

void mgws::_fn(struct mg_connection* c, int ev, void* ev_data, void* ctx)
{
	TRACE(__FUNCTION__);
	auto context = static_cast<mgws::context*>(ctx);

	auto self = context->_mgws;
	self.fn(c, ev, ev_data, context);
}

void mgws::fn(struct mg_connection* c, int ev, void* ev_data, context* ctx)
{
	TRACE(__FUNCTION__);
	if (MG_EV_ACCEPT == ev) {
		mg_tls_init(c, &m_tls_opts);
		return;
	}

	if (MG_EV_HTTP_MSG == ev) {
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;

		if (mg_http_match_uri(hm, "/websock")) {
			// upgrade to ws:/wss: connection, recv MG_EV_WS_MSG thereafter
			mg_ws_upgrade(c, hm, NULL);

			// TODO: Add a Session here...
		}
		else {
			mg_http_serve_dir(c, hm, &m_http_serve_opts);
		}
		return;
	}

	if (MG_EV_WS_MSG == ev) {
		TRACE(__FUNCTION__ << "() MG_EV_WS_MSG");
		return;
	}

	if (MG_EV_CLOSE == ev) {
		TRACE(__FUNCTION__ << "() MG_EV_CLOSE");
		// TODO: delete Session here
		return;
	}
}
