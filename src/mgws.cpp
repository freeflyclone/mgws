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
	m_root_dir(root)
{
	TRACE(root << ", " << cert << ", " << key);

	readPEM(cert, m_cert);
	readPEM(key, m_key);

	m_tls_opts.cert = mg_str(m_cert.c_str());
	m_tls_opts.key = mg_str(m_key.c_str());

	m_http_serve_opts.root_dir = m_root_dir.c_str();

	//mg_log_set(MG_LL_DEBUG);
	mg_mgr_init(&m_mgr);

	// Each new mg_connection initially gets "this" for its fn_data
	mg_http_listen(&m_mgr, "http://0.0.0.0:8443", mgws::_fn, this);
}

void mgws::infiniteLoop()
{
	while (true)
		mg_mgr_poll(&m_mgr, 1000);
}

void mgws::_fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	auto self = static_cast<mgws*>(fn_data);
	self->fn(c, ev, ev_data, fn_data);
}

void mgws::readPEM(const std::string& name, std::string& data) {
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

void mgws::fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	if (MG_EV_ACCEPT == ev) {
		mg_tls_init(c, &m_tls_opts);
		return;
	}

	if (MG_EV_HTTP_MSG == ev) {
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;

		if (mg_http_match_uri(hm, "/websock")) {
			// upgrade to ws:/wss: connection, recv MG_EV_WS_MSG thereafter
			mg_ws_upgrade(c, hm, NULL);
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
		return;
	}
}