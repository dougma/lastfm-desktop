/***************************************************************************
*   Copyright 2005-2008 Last.fm Ltd.                                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#ifndef WS_SENS_WIN_H
#define WS_SENS_WIN_H

#include <windows.h>
#import <stdole2.tlb>
#include <Sensevts.h>
#include <comadmin.h>
#include <atlbase.h>
#include <atlcom.h>


// Sens wraps up Windows' System Event Notification Service, and
// subscribes to ConnectionMade and ConnectionLost events

#define SENS_SUBSCRIPTIONS 2

class Sens : 
	public IDispatchImpl<ISensNetwork, &IID_ISensNetwork, &LIBID_SensEvents>
{
	CComPtr<ICOMAdminCatalog> m_pComAdmin;
	CComPtr<ICatalogCollection> m_pCatColl;
	CComVariant m_keys[SENS_SUBSCRIPTIONS];

	HRESULT addEvent(ICatalogCollection *pColl, const char *methodName, unsigned keyIdx);
	bool keyInKeys(const CComVariant &key);

public:
	void init();
	void uninit();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE ConnectionMade(BSTR bstrConnection, ULONG ulType, LPSENS_QOCINFO lpQOCInfo);
	HRESULT STDMETHODCALLTYPE ConnectionMadeNoQOCInfo(BSTR bstrConnection, ULONG ulType);
	HRESULT STDMETHODCALLTYPE ConnectionLost(BSTR bstrConnection, ULONG ulType);
	HRESULT STDMETHODCALLTYPE DestinationReachable(BSTR bstrDestination, BSTR bstrConnection, ULONG ulType, LPSENS_QOCINFO lpQOCInfo);
	HRESULT STDMETHODCALLTYPE DestinationReachableNoQOCInfo(BSTR bstrDestination, BSTR bstrConnection, ULONG ulType);


	// simply throwing replacement for HRESULT; 
	// throws when assigned failure error codes
	class HResult
	{
	public:
		HRESULT m_hr;
		const char *m_trying;

	public:
		HResult()
			: m_hr(0)
		{}

		HRESULT operator=(HRESULT hr)
		{
			m_hr = hr;
			if (FAILED(m_hr))
			{
				throw HResult(*this);
			}
			return m_hr;
		}

		void trying(const char *message)
		{
			m_trying = message;
		}
	};

};


#endif