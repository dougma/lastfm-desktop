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

class QVBoxLayout;

class SourceListWidget : public QWidget
{
    Q_OBJECT;

public:
    enum SourceType { Tag, Artist, User };
    enum Operator { And, Or, AndNot };

    SourceListWidget(int maxSources, QWidget* parent = 0);

    QString rql();
    QString stationDescription();
    bool addSource(SourceType type, const QString& name);

private slots:

private:
    typedef QPair<SourceType, QString> Source;

    void setOp(int sourceIdx);
    void setSource(int sourceIdx, QWidget* widget);
    void addPlaceholder();

    static Operator defaultOp(SourceType first, SourceType second);
    static QWidget* createWidget(SourceType type, const QString& name);

    QVBoxLayout* m_layout;
    int m_maxSources;
    QList<Source> m_sources;
};

#endif
