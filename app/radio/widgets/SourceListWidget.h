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

#ifndef SOURCE_LIST_WIDGET_H
#define SOURCE_LIST_WIDGET_H

#include <QWidget>
#include "../SourceListModel.h"

class QVBoxLayout;
class SourceItemWidget;

class SourceListWidget : public QWidget
{
    Q_OBJECT;

public:
    enum Operator { And, Or, AndNot };

    SourceListWidget(bool advanced, QWidget* parent = 0);
    void setModel(SourceListModel* model);

    QString rql();
    QString stationDescription();

private slots:
    void onDeleteClicked();

    void onRowsInserted(const QModelIndex&, int, int);
    void onRowsAboutToBeRemoved(const QModelIndex&, int, int);

private:
    void setOp(int sourceIdx);
    void addPlaceholder();
    void addPlaceholders();

    SourceItemWidget* createWidget(int index);
    static Operator defaultOp(RqlSource::Type first, RqlSource::Type second);

    QVBoxLayout* m_layout;
    SourceListModel* m_model;
    bool m_advanced;
};

#endif
