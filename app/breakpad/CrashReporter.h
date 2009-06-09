/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole, Erik Jaelevik, 
        Christian Muehlhaeuser

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CRASHREPORTER_H
#define CRASHREPORTER_H

#include <QDialog>
#include <QFile>

#include "ui_CrashReporter.h"


class CrashReporter : public QDialog
{
    Q_OBJECT

public:
    CrashReporter( const QStringList& argv );

private:
    Ui::CrashReporter ui;

    QString m_username;
    QString m_minidump;
    QString m_dir;
    QString m_product_name;
    class CachedHttp* m_http;

public slots:
    void send();

private slots:
    void onDone( const QByteArray& );
    void onProgress( int done, int total );
    void onFail( int error, const QString& errorString );
};

#endif // CRASHREPORTER_H
