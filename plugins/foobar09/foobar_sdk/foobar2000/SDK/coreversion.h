#ifndef _COREVERSION_H_
#define _COREVERSION_H_

class NOVTABLE core_version_info : public service_base {
public:
	virtual const char * get_version_string() = 0;
	static const char * g_get_version_string() {return static_api_ptr_t<core_version_info>()->get_version_string();}
	FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(core_version_info);
};

struct t_core_version_data {
	t_uint32 m_major, m_minor1, m_minor2, m_minor3;
};

//! New (0.9.4.2)
class NOVTABLE core_version_info_v2 : public core_version_info {
public:
	virtual const char * get_name() = 0;//"foobar2000"
	virtual const char * get_version_as_text() = 0;//"N.N.N.N"
	virtual t_core_version_data get_version() = 0;

	FB2K_MAKE_SERVICE_INTERFACE(core_version_info_v2, core_version_info);
};

#endif //_COREVERSION_H_
