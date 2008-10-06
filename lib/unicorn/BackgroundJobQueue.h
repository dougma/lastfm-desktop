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

#ifndef BACKGROUND_JOB_QUEUE_H
#define BACKGROUND_JOB_QUEUE_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include "BackgroundJob.h"


/** simple work thread, hardly any features, but it does delete the job for
  * you when it's done. Only one thread is spawned at this time */
class BackgroundJobQueue : public QThread
{
    Q_OBJECT
    
    QMutex mutex;
    QQueue<BackgroundJob*> q;
    
    virtual void run();
    
public:   
    /** we take ownership of the job and run it as soon as possible */
    void enqueue( BackgroundJob* );
};

#endif
