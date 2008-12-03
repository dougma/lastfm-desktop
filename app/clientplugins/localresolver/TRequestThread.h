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

#ifndef T_REQUEST_THREAD_H
#define T_REQUEST_THREAD_H

#include <QThread>
#include <QList>
#include <QMutex>
#include <QWaitCondition>


// TRequestThread queues up T* objects and calls doRequest 
// within the thread.

template <typename T>
class TRequestThread : public QThread
{
    QMutex m_mutex;
    QWaitCondition m_wakeUp;
    QList<T*> m_queue;
    bool m_stopping;

    virtual void doRequest(T* req) = 0;

protected:
    T* takeNextFromQueue()
    {
        T* t = 0;
        m_mutex.lock();
        if (!m_queue.isEmpty()) 
            t = m_queue.takeFirst();
        m_mutex.unlock();
        return t;
    }

    virtual void run()
    {
        while (!m_stopping) {
            m_mutex.lock();
            m_wakeUp.wait(&m_mutex);
            m_mutex.unlock();

            for(;;) {
                T* req = takeNextFromQueue();
                if (req == 0 || m_stopping) break;
                doRequest(req);
            }
        }
    }

public:
    TRequestThread()
    : m_stopping(false)
    {
    }

    ~TRequestThread()
    {
        m_stopping = true;
        m_mutex.lock();
        m_wakeUp.wakeAll();
        m_mutex.unlock();
        wait();
    }

    void enqueue(T* req)
    {
        Q_ASSERT(req);
        if (req) {
            m_mutex.lock();
            m_queue.append(req);
            m_wakeUp.wakeAll();
            m_mutex.unlock();
        }
    }

};

#endif