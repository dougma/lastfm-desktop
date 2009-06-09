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
#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <QWidget>
#include <QStack>

class QPushButton;
class QHBoxLayout;

class HistoryWidget : public QWidget
{
    Q_OBJECT

public:
    HistoryWidget(QWidget* parent = 0);
    ~HistoryWidget();

    bool pop();

signals:
    void clicked(int position, const QString& text);

public slots:
    void newItem(const QString& text);

private slots:
    void onItemClicked();

private:
    QHBoxLayout* m_layout;
    QStack<QPushButton*> m_labels;
};

#endif
