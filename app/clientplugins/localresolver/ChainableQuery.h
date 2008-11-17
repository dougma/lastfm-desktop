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

#ifndef CHAINABLE_QUERY_H
#define CHAINABLE_QUERY_H

#include <QSqlDatabase>
#include <QSqlQuery>

// ChainableQuery allows us to chain calls to 
// (QSqlQuery's) prepare(), bindValue() and exec().
//
// exec() ends the chain by returning a QSqlQuery
//
// All errors are thrown as type QSqlError

class ChainableQuery : public QSqlQuery
{
public:
    ChainableQuery(QSqlDatabase db);
    ChainableQuery prepare(const QString& sql);
    ChainableQuery bindValue(const QString& sql, const QVariant& v);
    ChainableQuery setForwardOnly(bool forward);
    QSqlQuery exec();
};

#endif