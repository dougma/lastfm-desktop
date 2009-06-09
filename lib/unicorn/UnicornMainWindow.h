/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#ifndef UNICORN_MAIN_WINDOW
#define UNICORN_MAIN_WINDOW


#include "lib/DllExportMacro.h"
#include <QMainWindow>
#include <QPointer>
#include <QDialog>
class AboutDialog;
class UpdateDialog;
class QNetworkReply;


template <typename D> struct OneDialogPointer : public QPointer<D>
{    
    OneDialogPointer& operator=( QDialog* d )
    {
        QPointer<D>::operator=( (D*)d );
    	d->setAttribute( Qt::WA_DeleteOnClose ); \
    	d->setWindowFlags( Qt::Dialog | Qt::WindowMinimizeButtonHint ); \
    	d->setModal( false );
        return *this;
    }
    
    void show()
    {
        QDialog* d = (QDialog*)QPointer<D>::data();
        d->show();
        d->raise();
        d->activateWindow();
    }
};


namespace unicorn
{
    class UNICORN_DLLEXPORT MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow();
        ~MainWindow();

        /** call this to add the account menu and about menu action, etc. */
        void finishUi();

    public slots:
        void about();
        void checkForUpdates();
        void visitProfile();
        void openLog();

    protected:
        struct Ui {
            Ui() : account( 0 ), profile( 0 )
            {}

            QMenu* account;
            QAction* profile;
            OneDialogPointer<UpdateDialog> update;
            OneDialogPointer<AboutDialog> about;
            
        } ui;

    private slots:
        void onUserGotInfo( QNetworkReply* );
    };
}

#endif
