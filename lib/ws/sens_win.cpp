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

#include "sens_win.h"
#include "sens.h"


void
Sens::init()
{
	HResult hr;
	hr.trying("CoInitialize");
	hr = CoInitialize(0);

	hr.trying("getting collection");
	{
		CComPtr<ICOMAdminCatalog> pComAdmin;
		hr = pComAdmin.CoCreateInstance(CLSID_COMAdminCatalog);
		hr = pComAdmin->GetCollection(CComBSTR("TransientSubscriptions"), (IDispatch**)&m_pCatColl);
	}

	hr.trying("subscribing to ConnectionMade");
	hr = addEvent(m_pCatColl, "ConnectionMade", 0);
	hr.trying("subscribing to ConnectionLost");
	hr = addEvent(m_pCatColl, "ConnectionLost", 1);

	hr.trying("saving subscription changes");
	LONG cChanges = 0;
	hr = m_pCatColl->SaveChanges(&cChanges);
}


void
Sens::uninit()
{
	// this is a bit messy. is there an easier way?
	// loop through the collection and remove them by key
	// (we noted their keys as we added them)
	if (m_pCatColl)
	{
		long count;
		HRESULT hr = m_pCatColl->get_Count(&count);
		if (SUCCEEDED(hr) && count)
		{
			// loop from high to low because removing objects re-indexes the collection
			// i've only ever seen our 2 subscriptions in this collection(!)
			for (long i = count-1; i >= 0; i--)
			{
				CComPtr<ICatalogObject> pObj;
				if (SUCCEEDED(m_pCatColl->get_Item(i, (IDispatch**) &pObj)))
				{
					CComVariant key;
					if (SUCCEEDED(pObj->get_Key(&key)) && keyInKeys(key)) 
					{
						hr = m_pCatColl->Remove(i);
					}
				}
			}
			LONG cChanges = 0;
			hr = m_pCatColl->SaveChanges(&cChanges);
		}
		m_pCatColl.Release(); // release before COM goes away!
		CoUninitialize();
	}
}

bool
Sens::keyInKeys(const CComVariant &key)
{
	for (int i = 0; i < SENS_SUBSCRIPTIONS; i++)
		if (m_keys[i] == key) return true;
	return false;
}

HRESULT 
Sens::addEvent(ICatalogCollection *pColl, const char *methodName, unsigned keyIdx)
{
	if (keyIdx >= SENS_SUBSCRIPTIONS) return E_INVALIDARG;

	CComPtr<ICatalogObject> pCatObj;
	HRESULT hr = pColl->Add((IDispatch**)&pCatObj);
	if (FAILED(hr)) return hr;

	hr = pCatObj->get_Key(&(m_keys[keyIdx]));
	if (FAILED(hr)) return hr;

	// "{d5978620-5b9f-11d1-8dd2-00aa004abd5e}" is SENSGUID_EVENTCLASS_NETWORK 
	hr = pCatObj->put_Value(CComBSTR("EventCLSID"), CComVariant(CComBSTR("{d5978620-5b9f-11d1-8dd2-00aa004abd5e}")));
	if (FAILED(hr)) return hr;

	hr = pCatObj->put_Value(CComBSTR("Name"), CComVariant(CComBSTR("last.fm")));
	if (FAILED(hr)) return hr;

	hr = pCatObj->put_Value(CComBSTR("MethodName"), CComVariant(CComBSTR(methodName)));
	if (FAILED(hr)) return hr;

	hr = pCatObj->put_Value(CComBSTR("SubscriberInterface"), CComVariant((IUnknown*)this));
	if (FAILED(hr)) return hr;

	hr = pCatObj->put_Value(CComBSTR("Enabled"), CComVariant(true));
	if (FAILED(hr)) return hr;

	return pCatObj->put_Value(CComBSTR("PerUser"), CComVariant(true));
}


HRESULT 
Sens::ConnectionMade(BSTR, ULONG, LPSENS_QOCINFO)
{
	return S_OK;
}

HRESULT 
Sens::ConnectionMadeNoQOCInfo(BSTR, ULONG)
{
	return S_OK;
}

HRESULT
Sens::ConnectionLost(BSTR, ULONG)
{
	return S_OK;
}

HRESULT
Sens::DestinationReachable(BSTR, BSTR, ULONG, LPSENS_QOCINFO)
{
	return S_OK;
}

HRESULT
Sens::DestinationReachableNoQOCInfo(BSTR, BSTR, ULONG)
{
	return S_OK;
}


//////

ULONG
Sens::AddRef()
{
	return 1;	// we'll manage our own lifetime thanks
}

ULONG
Sens::Release()
{
	return 1;	// we'll manage our own lifetime thanks
}

HRESULT
Sens::QueryInterface(REFIID riid, void **ppvObject)
{
	if ((riid == __uuidof(ISensNetwork)) || (riid == __uuidof(IDispatch)) || (riid == __uuidof(IUnknown)))
		*ppvObject = this;
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
	return S_OK;

}

