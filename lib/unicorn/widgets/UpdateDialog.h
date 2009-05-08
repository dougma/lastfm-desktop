/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#ifndef UPDATE_DIALOG_H 
#define UPDATE_DIALOG_H

#include "lib/DllExportMacro.h"
#include <lastfm/ws.h>
#include <QDialog>


/** we did use QProgressDialog, but it's so amazingly shit. We stopped.
  * GJ as usual Trolltech. */
class UNICORN_DLLEXPORT UpdateDialog : public QDialog
{
    Q_OBJECT
    
    QNetworkReply* checking;
    QByteArray md5;
    QUrl url;
    QString tmpFileName;
    
    class QLabel* text;
    class QProgressBar* bar;
    class QPushButton* button;

public:
    /** auto deletes if the check comes back false
      * call show() if you want the whole process to be visible */
    UpdateDialog( QWidget* parent );
    
private slots:
    void onGot();
    void onProgress( qint64 received, qint64 total );
    void onDownloaded();
    void install();
};

#endif
