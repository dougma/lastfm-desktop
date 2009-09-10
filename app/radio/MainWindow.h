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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lib/unicorn/UnicornMainWindow.h"
#include <lastfm/RadioStation>

class MessageBar;

class MainWindow : public unicorn::MainWindow
{
    Q_OBJECT

public:
    MainWindow();

signals:
    void startRadio(const RadioStation&);

public slots:
    void onRadioError(int code, const QVariant& data);

protected:
    virtual void resizeEvent(QResizeEvent* event);
    
private:
    MessageBar* m_messageBar;
};

#endif // MAINWINDOW_H
