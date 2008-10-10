class registerclass_scope_delayed {
public:
	registerclass_scope_delayed() : m_class(0) {}

	bool is_registered() const {return m_class != 0;}
	void toggle_on(UINT p_style,WNDPROC p_wndproc,int p_clsextra,int p_wndextra,HICON p_icon,HCURSOR p_cursor,HBRUSH p_background,const TCHAR * p_classname,const TCHAR * p_menuname);
	void toggle_off();
	ATOM get_class() const {return m_class;}

	~registerclass_scope_delayed() {toggle_off();}
private:
	registerclass_scope_delayed(const registerclass_scope_delayed &) {throw pfc::exception_not_implemented();}
	const registerclass_scope_delayed & operator=(const registerclass_scope_delayed &) {throw pfc::exception_not_implemented();}

	ATOM m_class;
};



template<typename t_object>
class syncd_storage {
private:
	typedef syncd_storage<t_object> t_self;
public:
	syncd_storage() {}
	template<typename t_source>
	syncd_storage(const t_source & p_source) : m_object(p_source) {}
	template<typename t_source>
	void set(t_source const & p_in) {
		insync(m_sync);
		m_object = p_in;
	}
	template<typename t_destination>
	void get(t_destination & p_out) const {
		insync(m_sync);
		p_out = m_object;
	}
	t_object get() const {
		insync(m_sync);
		return m_object;
	}
	template<typename t_source>
	const t_self & operator=(t_source const & p_source) {set(p_source); return *this;}
private:
	mutable critical_section m_sync;
	t_object m_object;
};

template<typename t_object>
class syncd_storage_flagged {
private:
	typedef syncd_storage_flagged<t_object> t_self;
public:
	syncd_storage_flagged() : m_changed_flag(false) {}
	template<typename t_source>
	syncd_storage_flagged(const t_source & p_source) : m_changed_flag(false), m_object(p_source) {}
	void set_changed(bool p_flag = true) {
		insync(m_sync); 
		m_changed_flag = p_flag;
	}
	template<typename t_source>
	void set(t_source const & p_in) {
		insync(m_sync);
		m_object = p_in;
		m_changed_flag = true;
	}
	bool has_changed() const {
		insync(m_sync); 
		return m_changed_flag;
	}
	t_object peek() const {insync(m_sync); return m_object;}
	template<typename t_destination>
	bool get_if_changed(t_destination & p_out) {
		insync(m_sync);
		if (m_changed_flag) {
			p_out = m_object;
			m_changed_flag = false;
			return true;
		} else {
			return false;
		}
	}
	t_object get() {
		insync(m_sync);
		m_changed_flag = false;
		return m_object;
	}
	template<typename t_destination>
	void get(t_destination & p_out) {
		insync(m_sync);
		p_out = m_object;
		m_changed_flag = false;
	}
	template<typename t_source>
	const t_self & operator=(t_source const & p_source) {set(p_source); return *this;}
private:
	bool m_changed_flag;
	mutable critical_section m_sync;
	t_object m_object;
};

class CGlobalLock {
public:
	CGlobalLock(HGLOBAL p_handle) : m_handle(p_handle), m_ptr(GlobalLock(p_handle)) {}
	~CGlobalLock() {
		if (m_ptr != NULL) GlobalUnlock(m_handle);
	}
	void * GetPtr() const {return m_ptr;}
	t_size GetSize() const {return GlobalSize(m_handle);}
private:
	void * m_ptr;
	HGLOBAL m_handle;
};

bool uGetClipboardString(pfc::string_base & p_out);



#ifdef __ATLWIN_H__

class CMenuSelectionReceiver : public CWindowImpl<CMenuSelectionReceiver> {
public:
	CMenuSelectionReceiver(HWND p_parent) {
		SetLastError(NO_ERROR);
		if (Create(p_parent) == NULL) throw exception_win32(GetLastError());
	}
	~CMenuSelectionReceiver() {
		DestroyWindow();
	}
	typedef CWindowImpl<CMenuSelectionReceiver> _baseClass;
	DECLARE_WND_CLASS_EX(TEXT("{DF0087DB-E765-4283-BBAB-6AB2E8AB64A1}"),0,0);

	BEGIN_MSG_MAP(CMenuSelectionReceiver)
		MESSAGE_HANDLER(WM_MENUSELECT,OnMenuSelect)
	END_MSG_MAP()
protected:
	virtual bool QueryHint(unsigned p_id,pfc::string_base & p_out) {
		return false;
	}
private:
	LRESULT OnMenuSelect(UINT,WPARAM p_wp,LPARAM p_lp,BOOL&) {
		if (p_lp != 0) {
			if (HIWORD(p_wp) & MF_POPUP) {
				m_status.release();
			} else {
				pfc::string8 msg;
				if (!QueryHint(LOWORD(p_wp),msg)) {
					m_status.release();
				} else {
					if (m_status.is_empty()) {
						if (!static_api_ptr_t<ui_control>()->override_status_text_create(m_status)) m_status.release();
					}
					if (m_status.is_valid()) {
						m_status->override_text(msg);
					}
				}
			}
		} else {
			m_status.release();
		}
		return 0;
	}

	service_ptr_t<ui_status_text_override> m_status;

	PFC_CLASS_NOT_COPYABLE(CMenuSelectionReceiver,CMenuSelectionReceiver);
};
#endif //#ifdef __ATLWIN_H__
