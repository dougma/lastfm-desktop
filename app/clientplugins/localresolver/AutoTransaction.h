/***************************************************************************
 *   Copyright 2009 Last.fm Ltd.                                           *
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

#ifndef AUTO_TRANSACTION_H
#define AUTO_TRANSACTION_H


// todo: add mutex support (see use of this with LocalCollection)
template<typename TDatabase>
class AutoTransaction
{
private:
    TDatabase& m_db;
    bool m_rollback;

public:
    AutoTransaction(TDatabase &db)
        : m_db(db)
        , m_rollback(true)
    {
        m_db.transactionBegin();
    }

    ~AutoTransaction()
    {
        if (m_rollback) {
            m_db.transactionRollback();
        } else {
            m_db.transactionCommit();
        }
    }

    void rollback()
    {
        m_rollback = true;
    }

    void commit()
    {
        m_rollback = false;
    }
};

#endif