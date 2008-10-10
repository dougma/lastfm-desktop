#include "stdafx.h"


using namespace InPlaceEdit;


static const TCHAR g_prop_instance[] = _T("{91154665-2B6A-42da-BB2A-A132EC20927F}");


namespace {

	static pfc::avltree_t<HWND> g_editboxes;
	static HHOOK g_hook = NULL;

	static LRESULT CALLBACK GMouseProc(int nCode,WPARAM wParam,LPARAM lParam) {
		if (nCode >= 0) {
			const MOUSEHOOKSTRUCT * mhs = (const MOUSEHOOKSTRUCT *) lParam;
			switch(wParam) {
			case WM_NCLBUTTONDOWN:
			case WM_NCRBUTTONDOWN:
			case WM_NCMBUTTONDOWN:
			case WM_NCXBUTTONDOWN:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_XBUTTONDOWN:
				if (!g_editboxes.have_item(mhs->hwnd)) {
					SetFocus(mhs->hwnd);
				}
				break;
			}
		}
		return CallNextHookEx(g_hook,nCode,wParam,lParam);
	}

	void on_editbox_creation(HWND p_editbox) {
		g_editboxes.add(p_editbox);
		if (g_hook == NULL) {
			g_hook = SetWindowsHookEx(WH_MOUSE,GMouseProc,NULL,GetCurrentThreadId());
		}
	}
	void on_editbox_destruction(HWND p_editbox) {
		g_editboxes.remove(p_editbox);
		if (g_editboxes.get_count() == 0 && g_hook != NULL) {
			UnhookWindowsHookEx(pfc::replace_null_t(g_hook));
		}
	}

	enum {
		MSG_COMPLETION = WM_USER,
		MSG_DISABLE_EDITING,
	};

class InPlaceEditHook {
public:
	InPlaceEditHook(HWND p_wnd) : m_oldproc(NULL) 
	{
		SetProp(p_wnd,g_prop_instance,reinterpret_cast<HANDLE>(this));
		m_oldproc = uHookWindowProc(p_wnd,GEditHook);
		on_editbox_creation(p_wnd);
	}
private:
	static void ForwardCompletion(HWND p_mywnd,unsigned p_code) {
		HWND owner = GetParent(p_mywnd);
		SendMessage(owner,MSG_DISABLE_EDITING,0,0);
		PostMessage(owner,MSG_COMPLETION,p_code,0);
		EnableWindow(p_mywnd,FALSE);
	}
	~InPlaceEditHook() {}
	LRESULT EditHook(HWND p_wnd,UINT p_msg,WPARAM p_wp,LPARAM p_lp) {
		switch(p_msg) {
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;
		case WM_KILLFOCUS:
			ForwardCompletion(p_wnd,KEditLostFocus);
			return m_oldproc(p_wnd,p_msg,p_wp,p_lp);
		case WM_CHAR:
			switch(p_wp) {
			case VK_RETURN:
				if (!IsKeyPressed(VK_LCONTROL) && !IsKeyPressed(VK_RCONTROL)) {
					ForwardCompletion(p_wnd,KEditEnter);
					return 0;
				}
				break;
			case VK_TAB:
				ForwardCompletion(p_wnd,IsKeyPressed(VK_SHIFT) ? KEditShiftTab : KEditTab);
				return 0;
			case VK_ESCAPE:
				ForwardCompletion(p_wnd,KEditAborted);
				return 0;
			}
			return m_oldproc(p_wnd,p_msg,p_wp,p_lp);
		case WM_DESTROY:
			{
				WNDPROC l_wndproc = m_oldproc;
				uHookWindowProc(p_wnd,l_wndproc);
				RemoveProp(p_wnd,g_prop_instance);
				on_editbox_destruction(p_wnd);
				try {delete this;} catch(...) {}
				return l_wndproc(p_wnd,p_msg,p_wp,p_lp);
			}
		default:
			return m_oldproc(p_wnd,p_msg,p_wp,p_lp);
		}
	}

	static LRESULT CALLBACK GEditHook(HWND p_wnd,UINT p_msg,WPARAM p_wp,LPARAM p_lp) {
		InPlaceEditHook * instance = reinterpret_cast<InPlaceEditHook*>( GetProp(p_wnd,g_prop_instance) );
		if (instance == NULL) return DefWindowProc(p_wnd,p_msg,p_wp,p_lp);
		return instance->EditHook(p_wnd,p_msg,p_wp,p_lp);
	}

	WNDPROC m_oldproc;
};

class InPlaceEditContainer {
public:
	InPlaceEditContainer(HWND p_parentwnd,const RECT & p_rect,unsigned p_flags,pfc::rcptr_t<pfc::string_base> p_content,completion_notify_ptr p_notify) 
		: m_content(p_content), m_notify(p_notify), m_completed(false), m_initialized(false), m_changed(false), m_disable_editing(false) {
		{
			static volatile unsigned increment;
			TCHAR classname[64];
			wsprintf(classname,_T("{54340C80-248C-4b8e-8CD4-D624A8E9377B}/%u"),++increment);
			m_class_scope.toggle_on(0,GWndProc,0,sizeof(void*),NULL,NULL,NULL,classname,NULL);
		}

		HWND container, edit;


		RECT rect_cropped;
		{
			RECT client;
			SetLastError(0);
			if (!GetClientRect(p_parentwnd,&client)) throw exception_win32(GetLastError());
			IntersectRect(&rect_cropped,&client,&p_rect);
		}
		AdjustWindowRect(&rect_cropped,WS_BORDER|WS_CHILD,FALSE);

		SetLastError(0);
		container = CreateWindowEx(
			0,
			(const TCHAR*) m_class_scope.get_class(),
			_T(""),
			WS_BORDER|WS_CHILD,
			rect_cropped.left,rect_cropped.top,
			rect_cropped.right-rect_cropped.left,rect_cropped.bottom-rect_cropped.top,
			p_parentwnd,NULL,
			core_api::get_my_instance(),reinterpret_cast<void*>(this));
		if (container == NULL) throw exception_win32(GetLastError());

		try {
			
			RECT parent_client;
			SetLastError(0);
			if (!GetClientRect(container,&parent_client)) throw exception_win32(GetLastError());
			SetLastError(0);
			edit = CreateWindowEx(
				/*WS_EX_STATICEDGE*/ 0,
				WC_EDIT,TEXT(""),
				((p_flags & KFlagMultiLine) ? (WS_VSCROLL|ES_MULTILINE) : ES_AUTOHSCROLL) | 
				((p_flags & KFlagReadOnly) ? ES_READONLY : 0) |
				WS_CHILD|ES_LEFT|WS_VISIBLE,//parent is invisible now
				0,0,
				parent_client.right,parent_client.bottom,
				container,
				(HMENU)ID_MYEDIT,
				core_api::get_my_instance(),
				0);

			if (edit == NULL) throw exception_win32(GetLastError());

			SendMessage(edit,WM_SETFONT,SendMessage(p_parentwnd,WM_GETFONT,0,0),0);

			uSetWindowText(edit,*m_content);
			SendMessage(edit,EM_SETSEL,0,-1);
		} catch(...) {
			PostMessage(container,MSG_COMPLETION,InPlaceEdit::KEditAborted,0);
			return;
		}

		try {
			new InPlaceEditHook(edit);
		} catch(...) {
			PostMessage(container,MSG_COMPLETION,InPlaceEdit::KEditAborted,0);
			return;
		}

		ShowWindow(container,SW_SHOW);
		SetFocus(edit);

		m_initialized = true;
	}
private:
	enum {ID_MYEDIT = 666};

	LRESULT WndProc(HWND p_wnd,UINT p_msg,WPARAM p_wp,LPARAM p_lp) {
		switch(p_msg) {
		case WM_MOUSEWHEEL:
			return 0;
		case MSG_DISABLE_EDITING:
			ShowWindow(p_wnd,SW_HIDE);
			UpdateWindow(GetParent(p_wnd));
			m_disable_editing = true;
			return 0;
		case MSG_COMPLETION:
			PFC_ASSERT(m_initialized);
			if ((p_wp & KEditMaskReason) != KEditLostFocus) {
				SetFocus(GetParent(p_wnd));
			}
			OnCompletion(p_wp);
			DestroyWindow(p_wnd);
			return 0;
		case WM_COMMAND:
			switch(p_wp) {
			case (EN_CHANGE << 16) | ID_MYEDIT:
				if (m_initialized && !m_disable_editing) {
					uGetDlgItemText(p_wnd,ID_MYEDIT,*m_content);
					m_changed = true;
				}
				return 0;
			default:
				return 0;
			}
			
		case WM_DESTROY:
			try {delete this;}catch(...){}
			SetWindowLongPtr(p_wnd,0,NULL);
			return DefWindowProc(p_wnd,p_msg,p_wp,p_lp);
		default:
			return DefWindowProc(p_wnd,p_msg,p_wp,p_lp);
		}
	}
	static LRESULT CALLBACK GWndProc(HWND p_wnd,UINT p_msg,WPARAM p_wp,LPARAM p_lp) {
		if (p_msg == WM_CREATE) {
			const CREATESTRUCT * ptr = reinterpret_cast<const CREATESTRUCT*>(p_lp);
			SetWindowLongPtr(p_wnd,0,reinterpret_cast<LONG_PTR>(ptr->lpCreateParams));
		}

		InPlaceEditContainer * instance = reinterpret_cast<InPlaceEditContainer*>(GetWindowLongPtr(p_wnd,0));
		if (instance != NULL) {
			return instance->WndProc(p_wnd,p_msg,p_wp,p_lp);
		} else {
			return DefWindowProc(p_wnd,p_msg,p_wp,p_lp);
		}
	}

	void OnCompletion(unsigned p_status) {
		if (!m_completed) {
			m_completed = true;
			p_status &= KEditMaskReason;
			unsigned code = p_status;
			if (m_changed && p_status != KEditAborted) code |= KEditFlagContentChanged;
			if (m_notify.is_valid()) m_notify->on_completion(code);
		}
	}

	registerclass_scope_delayed m_class_scope;
	pfc::rcptr_t<pfc::string_base> m_content;
	completion_notify_ptr m_notify;
	bool m_completed;
	bool m_initialized, m_changed;
	bool m_disable_editing;
};

}

static void fail(completion_notify_ptr p_notify) {
	completion_notify::g_signal_completion_async(p_notify,KEditAborted);
}

void InPlaceEdit::Start(HWND p_parentwnd,const RECT & p_rect,bool p_multiline,pfc::rcptr_t<pfc::string_base> p_content,completion_notify_ptr p_notify) {
	StartEx(p_parentwnd,p_rect,p_multiline ? KFlagMultiLine : 0, p_content,p_notify);
}

void InPlaceEdit::Start_FromListView(HWND p_listview,unsigned p_item,unsigned p_subitem,unsigned p_linecount,pfc::rcptr_t<pfc::string_base> p_content,completion_notify_ptr p_notify) {
	Start_FromListViewEx(p_listview,p_item,p_subitem,p_linecount,0,p_content,p_notify);
}

bool InPlaceEdit::TableEditAdvance(unsigned & p_item,unsigned & p_column, unsigned p_item_count,unsigned p_column_count, unsigned p_whathappened) {
	if (p_item >= p_item_count || p_column >= p_column_count) return false;
	int delta = 0;

	switch(p_whathappened & KEditMaskReason) {
	case KEditEnter:
		delta = (int) p_column_count;
		break;
	case KEditTab:
		delta = 1;
		break;
	case KEditShiftTab:
		delta = -1;
		break;
	default:
		return false;
	}
	while(delta > 0) {
		p_column++;
		if (p_column >= p_column_count) {
			p_column = 0;
			p_item++;
			if (p_item >= p_item_count) return false;
		}
		delta--;
	}
	while(delta < 0) {
		if (p_column == 0) {
			if (p_item == 0) return false;
			p_item--;
			p_column = p_column_count;
		} 
		p_column--;
		delta++;
	}
	return true;
}

void InPlaceEdit::StartEx(HWND p_parentwnd,const RECT & p_rect,unsigned p_flags,pfc::rcptr_t<pfc::string_base> p_content,completion_notify_ptr p_notify) {
	try {
		new InPlaceEditContainer(p_parentwnd,p_rect,p_flags,p_content,p_notify);
	} catch(...) {
		fail(p_notify);
	}
}

void InPlaceEdit::Start_FromListViewEx(HWND p_listview,unsigned p_item,unsigned p_subitem,unsigned p_linecount,unsigned p_flags,pfc::rcptr_t<pfc::string_base> p_content,completion_notify_ptr p_notify) {
	try {
		ListView_EnsureVisible(p_listview,p_item,FALSE);
		RECT itemrect;
		if (!ListView_GetSubItemRect(p_listview,p_item,p_subitem,LVIR_BOUNDS,&itemrect)) throw pfc::exception("ListView_GetSubItemRect failure");

		const bool multiline = p_linecount > 1;
		if (multiline) {
			itemrect.bottom = itemrect.top + (itemrect.bottom - itemrect.top) * p_linecount;
		}

		StartEx(p_listview,itemrect,p_flags | (multiline ? KFlagMultiLine : 0),p_content,p_notify);
	} catch(...) {
		fail(p_notify);
	}
}
