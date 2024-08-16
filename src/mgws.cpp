#include "mgws.h"

#include <fstream>
#include <sstream>
#include <exception>

mgws::mgws(
	const std::string& root,
	const std::string& addr_port,
	const std::string& cert,
	const std::string& key
)
	: m_context{ this, nullptr },
	m_mgr{},
	m_tls_opts{},
	m_http_serve_opts{},
	m_root_dir(root),
	m_addr_port(addr_port)
{
	TRACE(root << ", " << cert << ", " << key);

	read_pem(cert, m_cert);
	read_pem(key, m_key);

	m_tls_opts.cert = mg_str(m_cert.c_str());
	m_tls_opts.key = mg_str(m_key.c_str());

	m_http_serve_opts.root_dir = m_root_dir.c_str();

	mg_mgr_init(&m_mgr);

	mg_timer_add(&m_mgr, m_poll_interval_ms, MG_TIMER_REPEAT, _timer_event, this);

	// Each new mg_connection initially gets our "m_context" for its fn_data
	// Allows for both "this" and "mg_connection".
	// 
	// - "this" is for C mongoose callback static void mgws::_fn to establish
	//   the proper pointer for the per-instance mgws::fn() (note the "_" distinction)
	//
	// - "mg_connection" is for per connection use of mg_connection.fn_data to hold "Session" ptr.
	mg_http_listen(&m_mgr, m_addr_port.c_str(), mgws::_fn, &m_context);
}

void mgws::infinite_loop()
{
	while (true)
		mg_mgr_poll(&m_mgr, m_poll_interval_ms);
}

void mgws::_timer_event(void* ev_data) {
	auto m = (mgws*)ev_data;
	if (m)
		m->timer_event(mg_millis());
}

void mgws::timer_event(int64_t ms) {
	TRACE(__FUNCTION__ << "t: " << ms);
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
	((mgws::context*)ctx)->_mgws->fn(c, ev, ev_data, (mgws::context*)ctx);
}

void mgws::fn(struct mg_connection* c, int ev, void* ev_data, context* ctx)
{
	(void)ctx;
	if (MG_EV_HTTP_MSG == ev) {
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;
		mg_http_serve_dir(c, hm, &m_http_serve_opts);
		return;
	}
}
