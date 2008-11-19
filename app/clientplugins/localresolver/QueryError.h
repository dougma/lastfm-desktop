/***************************************************************************
 *   Copyright (C) 2007 by                                                 *
 *      Last.fm Ltd <client@last.fm>                                       *
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

#ifndef QUERY_ERROR_H
#define QUERY_ERROR_H

#include "ChainableQuery.h"
#include <QSqlError>

// extends QSqlError with more info about the source of the error
class QueryError : public QSqlError
{
    const char *m_func;
    QString m_sql;

public:
    QueryError(const QString& s)
        : QSqlError(s)
        , m_func("")
    {}

    QueryError(const QSqlError &error, const ChainableQuery& query)
        : QSqlError(error)
        , m_func(query.m_func)
        , m_sql(query.m_sql)
    {}

    const char* function() const 
    { 
        return m_func; 
    }

    QString sql() const 
    { 
        return m_sql; 
    }

    QString text() const 
    { 
        return QSqlError::text() + " function: " + function() + " sql: " + sql().simplified();
    }
};

#endif