/////////////////////////////////////////////////////////////////////////////
//
// wmp_scrobbler.h : Declaration of the CWmp_scrobbler
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __WMP_SCROBBLER_H_
#define __WMP_SCROBBLER_H_

#pragma once

#include "RegistryUtils.h"
#include "resource.h"
#include "wmpplug.h"
#include "Scrobbler.h"
#include "wmpBootStrap.h"

#define AUDIOSCROBBLER_APPNAME					    "Audioscrobbler Windows Media Player plugin"
#define AUDIOSCROBBLER_APPID						"wmp"
#define AUDIOSCROBBLER_REGKEY						"Software\\Audioscrobbler WMP Plugin\\"

// {E7C0F450-9B76-4481-B3DF-B3447B383762}
DEFINE_GUID(CLSID_Wmp_scrobbler, 0xE7C0F450, 0x9B76, 0x4481, 0xB3, 0xDF, 0xB3, 0x44, 0x7B, 0x38, 0x37, 0x62);

/////////////////////////////////////////////////////////////////////////////
// CWmp_scrobbler
class ATL_NO_VTABLE CWmp_scrobbler : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CWmp_scrobbler, &CLSID_Wmp_scrobbler>,
    public IWMPEvents,
    public IWMPPluginUI,
	public CScrobbler
{
public:
    CWmp_scrobbler();
    ~CWmp_scrobbler();

DECLARE_REGISTRY_RESOURCEID(IDR_WMP_SCROBBLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWmp_scrobbler)
    COM_INTERFACE_ENTRY(IWMPEvents)
    COM_INTERFACE_ENTRY(IWMPPluginUI)
END_COM_MAP()


		// SCROBBLER INTERFACE
		virtual 	CStdString	GetAppName(){ return AUDIOSCROBBLER_APPNAME;}
		virtual 	CStdString	GetAppId(){ return AUDIOSCROBBLER_APPID;}
		virtual 	CStdString	GetRegKey(){ return AUDIOSCROBBLER_REGKEY;}
		virtual		CStdString	GetPlayerVersion();	// Returns the player specific version string
		virtual		BOOL		IsPlaying();
		virtual		BOOL		IsPaused();
		virtual		BOOL		IsStopped();
		virtual		CStdString	GetCurrentSongFileName();
		virtual		int			GetTrackLength();
		virtual		int			GetTrackPosition();
		virtual		BOOL	    GetCurrentSong(SONG_INFO* pSong);
		virtual		HWND		GetParentWnd(){return m_hWndParent;}
//		BOOL			SupportedFileType(CStdString strPath);

		HWND		m_hWndParent;
		BOOL		m_bNewSong;

    // CComCoClass methods
    HRESULT FinalConstruct();
    void    FinalRelease();

    // IWMPPluginUI methods
    STDMETHODIMP SetCore(IWMPCore *pCore);
    STDMETHODIMP Create(HWND hwndParent, HWND *phwndWindow) { return E_NOTIMPL; }
    STDMETHODIMP Destroy() { return E_NOTIMPL; }
    STDMETHODIMP TranslateAccelerator(LPMSG lpmsg) { return E_NOTIMPL; }
    STDMETHODIMP DisplayPropertyPage(HWND hwndParent);
    STDMETHODIMP GetProperty(const WCHAR *pwszName, VARIANT *pvarProperty);
    STDMETHODIMP SetProperty(const WCHAR *pwszName, const VARIANT *pvarProperty);

    // IWMPEvents methods
    void STDMETHODCALLTYPE OpenStateChange( long NewState );
    void STDMETHODCALLTYPE PlayStateChange( long NewState );
    void STDMETHODCALLTYPE AudioLanguageChange( long LangID );
    void STDMETHODCALLTYPE StatusChange();
    void STDMETHODCALLTYPE ScriptCommand( BSTR scType, BSTR Param );
    void STDMETHODCALLTYPE NewStream();
    void STDMETHODCALLTYPE Disconnect( long Result );
    void STDMETHODCALLTYPE Buffering( VARIANT_BOOL Start );
    void STDMETHODCALLTYPE Error();
    void STDMETHODCALLTYPE Warning( long WarningType, long Param, BSTR Description );
    void STDMETHODCALLTYPE EndOfStream( long Result );
    void STDMETHODCALLTYPE PositionChange( double oldPosition, double newPosition);
    void STDMETHODCALLTYPE MarkerHit( long MarkerNum );
    void STDMETHODCALLTYPE DurationUnitChange( long NewDurationUnit );
    void STDMETHODCALLTYPE CdromMediaChange( long CdromNum );
    void STDMETHODCALLTYPE PlaylistChange( IDispatch * Playlist, WMPPlaylistChangeEventType change );
    void STDMETHODCALLTYPE CurrentPlaylistChange( WMPPlaylistChangeEventType change );
    void STDMETHODCALLTYPE CurrentPlaylistItemAvailable( BSTR bstrItemName );
    void STDMETHODCALLTYPE MediaChange( IDispatch * Item );
    void STDMETHODCALLTYPE CurrentMediaItemAvailable( BSTR bstrItemName );
    void STDMETHODCALLTYPE CurrentItemChange( IDispatch *pdispMedia);
    void STDMETHODCALLTYPE MediaCollectionChange();
    void STDMETHODCALLTYPE MediaCollectionAttributeStringAdded( BSTR bstrAttribName,  BSTR bstrAttribVal );
    void STDMETHODCALLTYPE MediaCollectionAttributeStringRemoved( BSTR bstrAttribName,  BSTR bstrAttribVal );
    void STDMETHODCALLTYPE MediaCollectionAttributeStringChanged( BSTR bstrAttribName, BSTR bstrOldAttribVal, BSTR bstrNewAttribVal);
    void STDMETHODCALLTYPE PlaylistCollectionChange();
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistAdded( BSTR bstrPlaylistName);
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistRemoved( BSTR bstrPlaylistName);
    void STDMETHODCALLTYPE PlaylistCollectionPlaylistSetAsDeleted( BSTR bstrPlaylistName, VARIANT_BOOL varfIsDeleted);
    void STDMETHODCALLTYPE ModeChange( BSTR ModeName, VARIANT_BOOL NewValue);
    void STDMETHODCALLTYPE MediaError( IDispatch * pMediaObject);
    void STDMETHODCALLTYPE OpenPlaylistSwitch( IDispatch *pItem );
    void STDMETHODCALLTYPE DomainChange( BSTR strDomain);
    void STDMETHODCALLTYPE SwitchedToPlayerApplication();
    void STDMETHODCALLTYPE SwitchedToControl();
    void STDMETHODCALLTYPE PlayerDockedStateChange();
    void STDMETHODCALLTYPE PlayerReconnect();
    void STDMETHODCALLTYPE Click( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE DoubleClick( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE KeyDown( short nKeyCode, short nShiftState );
    void STDMETHODCALLTYPE KeyPress( short nKeyAscii );
    void STDMETHODCALLTYPE KeyUp( short nKeyCode, short nShiftState );
    void STDMETHODCALLTYPE MouseDown( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE MouseMove( short nButton, short nShiftState, long fX, long fY );
    void STDMETHODCALLTYPE MouseUp( short nButton, short nShiftState, long fX, long fY );

    TCHAR        m_szPluginText[MAX_PATH];

private:
    void         ReleaseCore();

    CComPtr<IWMPCore>           m_spCore;
    CComPtr<IConnectionPoint>   m_spConnectionPoint;
    DWORD                       m_dwAdviseCookie;
    wmpBootStrap                m_wmpBootStrap;
};

#endif //__WMP_SCROBBLER_H_
