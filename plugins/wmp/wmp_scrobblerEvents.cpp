/////////////////////////////////////////////////////////////////////////////
//
// wmp_scrobblerEvents.cpp : Implementation of CWmp_scrobbler events
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wmp_scrobbler.h"

void CWmp_scrobbler::OpenStateChange( long NewState )
{
    switch (NewState)
    {
    case wmposUndefined:
        break;
	case wmposPlaylistChanging:
        break;
	case wmposPlaylistLocating:
        break;
	case wmposPlaylistConnecting:
        break;
	case wmposPlaylistLoading:
        break;
	case wmposPlaylistOpening:
        break;
	case wmposPlaylistOpenNoMedia:
        break;
	case wmposPlaylistChanged:
        break;
	case wmposMediaChanging:
        break;
	case wmposMediaLocating:
        break;
	case wmposMediaConnecting:
        break;
	case wmposMediaLoading:
        break;
	case wmposMediaOpening:
        break;
	case wmposMediaOpen:
        break;
	case wmposBeginCodecAcquisition:
        break;
	case wmposEndCodecAcquisition:
        break;
	case wmposBeginLicenseAcquisition:
        break;
	case wmposEndLicenseAcquisition:
        break;
	case wmposBeginIndividualization:
        break;
	case wmposEndIndividualization:
        break;
	case wmposMediaWaiting:
        break;
	case wmposOpeningUnknownURL:
        break;
    default:
        break;
    }
}

void CWmp_scrobbler::PlayStateChange( long NewState )
{
    switch (NewState)
    {
    case wmppsUndefined:
        break;
	case wmppsStopped:
				m_bNewSong = FALSE;
        break;
	case wmppsPaused:
        break;
	case wmppsPlaying:
				if(!m_bNewSong)
				{
					m_bNewSong = TRUE;
					OnTrackPlay();
				}
        break;
	case wmppsScanForward:
        break;
	case wmppsScanReverse:
        break;
	case wmppsBuffering:
        break;
	case wmppsWaiting:
        break;
	case wmppsMediaEnded:
				m_bNewSong = FALSE;
        break;
	case wmppsTransitioning:
				m_bNewSong = FALSE;
        break;
	case wmppsReady:
        break;
	case wmppsReconnecting:
        break;
	case wmppsLast:
        break;
    default:
        break;
    }
}

void CWmp_scrobbler::AudioLanguageChange( long LangID )
{
}

void CWmp_scrobbler::StatusChange()
{
}

void CWmp_scrobbler::ScriptCommand( BSTR scType, BSTR Param )
{
}

void CWmp_scrobbler::NewStream()
{
}

void CWmp_scrobbler::Disconnect( long Result )
{
}

void CWmp_scrobbler::Buffering( VARIANT_BOOL Start )
{
}

void CWmp_scrobbler::Error()
{
    CComPtr<IWMPError>      spError;
    CComPtr<IWMPErrorItem>  spErrorItem;
    HRESULT                 dwError = S_OK;
    HRESULT                 hr = S_OK;

    if (m_spCore)
    {
        hr = m_spCore->get_error(&spError);

        if (SUCCEEDED(hr))
        {
            hr = spError->get_item(0, &spErrorItem);
        }

        if (SUCCEEDED(hr))
        {
            hr = spErrorItem->get_errorCode( (long *) &dwError );
        }
    }

		PRINTF(DEBUG_INFO, "CWmp_scrobbler::Error", "Error code is %d", dwError );
}

void CWmp_scrobbler::Warning( long WarningType, long Param, BSTR Description )
{
}

void CWmp_scrobbler::EndOfStream( long Result )
{
}

void CWmp_scrobbler::PositionChange( double oldPosition, double newPosition)
{
}

void CWmp_scrobbler::MarkerHit( long MarkerNum )
{
}

void CWmp_scrobbler::DurationUnitChange( long NewDurationUnit )
{
}

void CWmp_scrobbler::CdromMediaChange( long CdromNum )
{
}

void CWmp_scrobbler::PlaylistChange( IDispatch * Playlist, WMPPlaylistChangeEventType change )
{
    switch (change)
    {
    case wmplcUnknown:
        break;
	case wmplcClear:
        break;
	case wmplcInfoChange:
        break;
	case wmplcMove:
        break;
	case wmplcDelete:
        break;
	case wmplcInsert:
        break;
	case wmplcAppend:
        break;
	case wmplcPrivate:
        break;
	case wmplcNameChange:
        break;
	case wmplcMorph:
        break;
	case wmplcSort:
        break;
	case wmplcLast:
        break;
    default:
        break;
    }
}

void CWmp_scrobbler::CurrentPlaylistChange( WMPPlaylistChangeEventType change )
{
    switch (change)
    {
    case wmplcUnknown:
        break;
	case wmplcClear:
        break;
	case wmplcInfoChange:
        break;
	case wmplcMove:
        break;
	case wmplcDelete:
        break;
	case wmplcInsert:
        break;
	case wmplcAppend:
        break;
	case wmplcPrivate:
        break;
	case wmplcNameChange:
        break;
	case wmplcMorph:
        break;
	case wmplcSort:
        break;
	case wmplcLast:
        break;
    default:
        break;
    }
}

void CWmp_scrobbler::CurrentPlaylistItemAvailable( BSTR bstrItemName )
{
}

void CWmp_scrobbler::MediaChange( IDispatch * Item )
{
}

void CWmp_scrobbler::CurrentMediaItemAvailable( BSTR bstrItemName )
{
}

void CWmp_scrobbler::CurrentItemChange( IDispatch *pdispMedia)
{
}

void CWmp_scrobbler::MediaCollectionChange()
{
}

void CWmp_scrobbler::MediaCollectionAttributeStringAdded( BSTR bstrAttribName,  BSTR bstrAttribVal )
{
}

void CWmp_scrobbler::MediaCollectionAttributeStringRemoved( BSTR bstrAttribName,  BSTR bstrAttribVal )
{
}

void CWmp_scrobbler::MediaCollectionAttributeStringChanged( BSTR bstrAttribName, BSTR bstrOldAttribVal, BSTR bstrNewAttribVal)
{
}

void CWmp_scrobbler::PlaylistCollectionChange()
{
}

void CWmp_scrobbler::PlaylistCollectionPlaylistAdded( BSTR bstrPlaylistName)
{
}

void CWmp_scrobbler::PlaylistCollectionPlaylistRemoved( BSTR bstrPlaylistName)
{
}

void CWmp_scrobbler::PlaylistCollectionPlaylistSetAsDeleted( BSTR bstrPlaylistName, VARIANT_BOOL varfIsDeleted)
{
}

void CWmp_scrobbler::ModeChange( BSTR ModeName, VARIANT_BOOL NewValue)
{
}

void CWmp_scrobbler::MediaError( IDispatch * pMediaObject)
{
}

void CWmp_scrobbler::OpenPlaylistSwitch( IDispatch *pItem )
{
}

void CWmp_scrobbler::DomainChange( BSTR strDomain)
{
}

void CWmp_scrobbler::SwitchedToPlayerApplication()
{
}

void CWmp_scrobbler::SwitchedToControl()
{
}

void CWmp_scrobbler::PlayerDockedStateChange()
{
}

void CWmp_scrobbler::PlayerReconnect()
{
}

void CWmp_scrobbler::Click( short nButton, short nShiftState, long fX, long fY )
{
}

void CWmp_scrobbler::DoubleClick( short nButton, short nShiftState, long fX, long fY )
{
}

void CWmp_scrobbler::KeyDown( short nKeyCode, short nShiftState )
{
}

void CWmp_scrobbler::KeyPress( short nKeyAscii )
{
}

void CWmp_scrobbler::KeyUp( short nKeyCode, short nShiftState )
{
}

void CWmp_scrobbler::MouseDown( short nButton, short nShiftState, long fX, long fY )
{
}

void CWmp_scrobbler::MouseMove( short nButton, short nShiftState, long fX, long fY )
{
}

void CWmp_scrobbler::MouseUp( short nButton, short nShiftState, long fX, long fY )
{
}
