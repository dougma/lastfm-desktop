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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "ChainableQuery.h"
#include "QueryError.h"


ChainableQuery::ChainableQuery(QSqlDatabase db)
:QSqlQuery(db)
{
}

ChainableQuery 
ChainableQuery::prepare(const QString& sql, const char *funcName)
{
    m_sql = sql;
    m_func = funcName;

    if (!QSqlQuery::prepare(sql))
        throw QueryError(lastError(), *this);
    return *this;
}

ChainableQuery 
ChainableQuery::bindValue(const QString& name, const QVariant& value)
{
    QSqlQuery::bindValue(name, value);
    return *this;
}

ChainableQuery 
ChainableQuery::setForwardOnly(bool forward)
{
    QSqlQuery::setForwardOnly(forward);
    return *this;
}

QSqlQuery
ChainableQuery::exec()
{
    if (!QSqlQuery::exec())
        throw QueryError(lastError(), *this);
    return *this;
}


QSqlQuery 
ChainableQuery::execBatch(QSqlQuery::BatchExecutionMode mode /*= QSqlQuery::ValuesAsRows*/)
{
    if (!QSqlQuery::execBatch(mode))
        throw QueryError(lastError(), *this);
    return *this;
}

