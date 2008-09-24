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

#ifndef WS_NET_EVENTS_H
#define WS_NET_EVENTS_H

#include <QtCore>


#ifdef WIN32

#include "sens_win.h"

// adapts windows-specific class to QT

class WsNetEventAdapter : 
	public QObject,
	public Sens			// the windows class
{
	Q_OBJECT

public:
	WsNetEventAdapter(QObject *parent = 0)
		:QObject(parent)
	{
		try
		{
			init();
		}
		catch (const HResult &hr)
		{
			qDebug() << "error " << hr.m_hr << " trying " << hr.m_trying << " in WsNetEventAdapter::init()";
		}
		catch (...)
		{
			qDebug() << "unhandled exception in WsNetEventAdapter::init()";
		}
	}

	~WsNetEventAdapter()
	{
		uninit();
	}

	HRESULT STDMETHODCALLTYPE ConnectionMade(BSTR bstrConnection, ULONG ulType, LPSENS_QOCINFO)
	{
		emit connectionUp(QString::fromUtf16(bstrConnection), ulType == 1);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ConnectionLost(BSTR bstrConnection, ULONG ulType)
	{
		emit connectionDown(QString::fromUtf16(bstrConnection), ulType == 1);
		return S_OK;
	}

signals:
	void connectionUp(QString connectionName, bool isWan);
	void connectionDown(QString connectionName, bool isWan);
};

#else

class WsNetEventAdapter:
	public QObject
{
public:
	WsNetEventAdapter(QObject *parent = 0)
		:QObject(parent)
	{
	}

signals:
	void connectionUp(QString connectionName, bool isWan);
	void connectionDown(QString connectionName, bool isWan);
};

#endif



class WsNetEvent : 
	public QObject
{
	Q_OBJECT

	WsNetEventAdapter *m_adapter;

public:
	WsNetEvent(QObject *parent = 0);

signals:
	void connectionUp(QString connectionName, bool isWan);
	void connectionDown(QString connectionName, bool isWan);
};

#endif