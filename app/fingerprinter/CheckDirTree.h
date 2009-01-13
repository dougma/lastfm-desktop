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

#ifndef CHECKDIRMODEL_H
#define CHECKDIRMODEL_H

#include <QDirModel>
#include <QTreeView>


class CheckDirModel : public QDirModel
{
    Q_OBJECT

    public:

        CheckDirModel(
            QWidget* parent = NULL) : QDirModel(parent) { }

        virtual Qt::ItemFlags
        flags(
            const QModelIndex& index) const;

        virtual QVariant
        data(
            const QModelIndex & index,
            int role = Qt::DisplayRole ) const;

        virtual bool
        setData(
            const QModelIndex & index,
            const QVariant & value,
            int role = Qt::EditRole);

        void
        setCheck(
            const QModelIndex& index,
            const QVariant& value);

        Qt::CheckState
        getCheck(
            const QModelIndex& index);

    signals:

        void 
        dataChangedByUser(
            const QModelIndex & index);

    private:

        QHash<qint64, Qt::CheckState> m_checkTable;

};

class CheckDirTree : public QTreeView
{
    Q_OBJECT

    public:

        CheckDirTree(
            QWidget* parent);
            
        void init();

        void
		checkPath( QString path,
               Qt::CheckState state );
               
        void expandPath( QString path );
        
        void showPath( QString path, QAbstractItemView::ScrollHint = QAbstractItemView::PositionAtTop );

        void
        setExclusions(
            QStringList list);

        QStringList
        getExclusions();
        
        void
        setInclusions(
            QStringList list );
        
        QStringList
        getInclusions();
        
        /*
            This will guess (sophistically) the path to the user's music library folder.
            It will default on the user's home folder.
            Returns true if it could guess a folder, otherwise, on default, it will return false.
        */
        bool getMusicLibraryPath( QString& musicPath );

    signals:

        void
        dataChanged();

    private:

        CheckDirModel m_dirModel;
        QSet<qint64>  m_expandedSet;

        void
        fillDown(
            const QModelIndex& index);

        void
        updateParent(
            const QModelIndex& index);

        void
        getExclusionsForNode(
            const QModelIndex& index,
            QStringList&       exclusions);
            
        void
        getInclusionsForNode(
            const QModelIndex& index,
            QStringList&       inclusions );

    private slots:

        void
        onCollapse(
            const QModelIndex& idx);

        void
        onExpand(
            const QModelIndex& idx);

        void
        updateNode(
            const QModelIndex& idx);

};

#endif // CHECKDIRTREE_H
