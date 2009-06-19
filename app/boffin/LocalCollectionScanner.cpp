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

#include "LocalCollectionScanner.h"

#include <QDebug>

using namespace std;

LocalCollectionScanner::LocalCollectionScanner(QObject* parent)
    : QObject(parent)
{
}

void 
LocalCollectionScanner::run(QDir playdarBinDir, QString collectionDbFilename, QStringList directories)
{
    QStringList args;
    args << collectionDbFilename;
    args << directories;

    m_proc = new QProcess(this);
    connect(m_proc, SIGNAL(readyReadStandardOutput()), SLOT(onReadyReadStandardOutput()));
    connect(m_proc, SIGNAL(readyReadStandardError()), SLOT(onReadyReadStandardError()));
    connect(m_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(onFinished(int, QProcess::ExitStatus)));
    connect(m_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(onError(QProcess::ProcessError)));
    m_proc->start(playdarBinDir.filePath("scanner.exe"), args);
}

void
LocalCollectionScanner::onReadyReadStandardOutput()
{
    QByteArray ba = m_proc->readAllStandardOutput();
    m_buffer.write(ba.constData(), ba.size());
    while (true) {
        string line;
        getline(m_buffer, line);
        if (m_buffer.fail()) {
            // the line is incomplete (for now)
            m_buffer.clear(); // clears the failbit
            return;
        } 
        LocalCollectionScanner::line(line);
    }
}

void
LocalCollectionScanner::onReadyReadStandardError()
{
}

void  
LocalCollectionScanner::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit finished();
}

void
LocalCollectionScanner::onError(QProcess::ProcessError)
{
    emit finished();
}

void
LocalCollectionScanner::line(const string& s)
{
    QString line = QString::fromUtf8(s.data(), s.length()).trimmed();
    QStringList fields = line.split(QChar('\t'), QString::KeepEmptyParts, Qt::CaseInsensitive);
    if (fields.size()) {
        if (fields[0] == "DIR:") {
            if (fields.size() > 1) {
                emit directory(fields[1]);
            }
        } else if (fields[0] == "TRACK:") {
            if (fields.size() > 4) {
                lastfm::Track t;
                lastfm::MutableTrack mt(t);
                mt.setArtist(fields[1]);
                mt.setAlbum(fields[2]);
                mt.setTitle(fields[3]);
                mt.setUrl(fields[4]);
                emit track(t);
            }
        }
    }
}

