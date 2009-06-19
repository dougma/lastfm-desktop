/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#ifndef LOCAL_COLLECTION_SCANNER_H
#define LOCAL_COLLECTION_SCANNER_H

#include <sstream>
#include <QDir>
#include <QProcess>
#include <QStringList>
#include <lastfm/Track>


class LocalCollectionScanner : public QObject
{
    Q_OBJECT;

public:
    LocalCollectionScanner(QObject* parent);
    void run(QDir playdarBinDir, QString collectionDbFilename, QStringList directories);

signals:
    void track(Track);
    void directory(QString);
    void finished();

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onFinished(int, QProcess::ExitStatus);
    void onError(QProcess::ProcessError);

private:
    void line(const std::string& line);

    QProcess* m_proc;
    QString m_collectionDbFilename;
    QDir m_playdarBinDir;
    std::stringstream m_buffer;
};

#endif
