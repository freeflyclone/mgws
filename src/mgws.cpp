#include "mgws.h"

mgws::mgws(
	const std::string& root,
	const std::string& cert,
	const std::string& key
)
	: m_mgr{},
	m_opts{},
	m_root_dir(root)
{
	TRACE(root << ", " << cert << ", " << key);

	//mg_log_set(MG_LL_DEBUG);
	mg_mgr_init(&m_mgr);

	// Each new session initially gets "this" for its fn_data
	mg_http_listen(&m_mgr, "http://0.0.0.0:80", mgws::fn, this);
}

void mgws::infiniteLoop()
{
	TRACE(__FUNCTION__);
	while (true)
		mg_mgr_poll(&m_mgr, 1000);
}

void mgws::fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	TRACE(__FUNCTION__ << "(), ev: " << ev);
}

const char* mgws::readPEM(char* name) {
	return nullptr;
}