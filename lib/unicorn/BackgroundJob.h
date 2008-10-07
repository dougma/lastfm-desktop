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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef BACKGROUND_JOB_H
#define BACKGROUND_JOB_H

#include <QObject>
#include "lib/DllExportMacro.h"


class UNICORN_DLLEXPORT BackgroundJob : public QObject
{
    Q_OBJECT
    friend class BackgroundJobQueue;

protected:
    /** reimplement if you want to prevent jobs running if certain criteria
      * are not met, you will be deleted in the GUI thread immediately if you 
      * return false */
    virtual bool isValid() const { return true; }
    /** do the work here, it will be called from teh queue thread */
    virtual void run() = 0;

signals:
    void finished();
    
protected slots:
    /** called from the GUI thread */
    virtual void onFinished()
    {}
};

#endif
