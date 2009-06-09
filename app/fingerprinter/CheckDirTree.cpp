/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Peter Grundstrom and Adam Renburg

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
#include <QHeaderView>
#include <QSettings>

#include "CheckDirTree.h"
#include "libLastFmTools/logger.h"

#ifdef WIN32
    #ifndef UNICODE
        #define UNICODE
    #endif

    #include "windows.h"
    #include "ShFolder.h"
#endif

/******************************************************************************
    CheckDirModel::flags
******************************************************************************/
Qt::ItemFlags
CheckDirModel::flags(
    const QModelIndex& index) const
{
    return QDirModel::flags(index) | Qt::ItemIsUserCheckable;
}

/******************************************************************************
    CheckDirModel::data
******************************************************************************/
QVariant
CheckDirModel::data(
    const QModelIndex& index,
    int role) const
{
    if (role == Qt::CheckStateRole)
    {
        int id = index.internalId();
        return m_checkTable.contains(id) ? m_checkTable.value(id) : Qt::Unchecked;
    }
    else
    {
        return QDirModel::data(index, role);
    }
}    

/******************************************************************************
    CheckDirModel::setData
    
    Gets called when the user checks/unchecks through the GUI.
******************************************************************************/
bool
CheckDirModel::setData(
    const QModelIndex& index,
    const QVariant& value,
    int   role)
{
    if (role == Qt::CheckStateRole)
    {
        m_checkTable.insert(index.internalId(), (Qt::CheckState)value.toInt());

        emit dataChanged(index, index);
        emit dataChangedByUser(index);
        
        return true;
    }
    else
    {
        return QDirModel::setData(index, value, role);
    }
}    

/******************************************************************************
    CheckDirModel::setCheck
    
    Use for programmatically setting check state.
******************************************************************************/
void
CheckDirModel::setCheck(
    const QModelIndex& index,
    const QVariant& value)
{
    m_checkTable.insert(index.internalId(), (Qt::CheckState)value.toInt());
    emit dataChanged(index, index);
}    

/******************************************************************************
    CheckDirModel::getCheck
******************************************************************************/
Qt::CheckState
CheckDirModel::getCheck(
    const QModelIndex& index)
{
    return (Qt::CheckState)data(index, Qt::CheckStateRole).toInt();
}    

/******************************************************************************
    CheckDirTree::CheckDirTree
******************************************************************************/
CheckDirTree::CheckDirTree(
    QWidget* parent) :
        QTreeView(parent)
{
    m_dirModel.setFilter( QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden );
    setModel(&m_dirModel);
    setColumnHidden(1, true);
    setColumnHidden(2, true);
    setColumnHidden(3, true);
    header()->hide();
    
    connect(&m_dirModel, SIGNAL(dataChangedByUser(const QModelIndex&)),
            this,        SLOT  (updateNode(const QModelIndex&)));

    connect(&m_dirModel, SIGNAL(dataChangedByUser(const QModelIndex&)),
            this,        SIGNAL(dataChanged()));

    connect(this, SIGNAL(collapsed(const QModelIndex&)),
            this, SLOT  (onCollapse(const QModelIndex&)));

    connect(this, SIGNAL(expanded(const QModelIndex&)),
            this, SLOT  (onExpand(const QModelIndex&)));

}

/******************************************************************************
    CheckDirTree::init
******************************************************************************/
void CheckDirTree::init()
{
    QString musicPath;
    bool found = getMusicLibraryPath( musicPath );
    
    showPath( musicPath );
    expandPath( musicPath );
    if ( found )
        checkPath( musicPath, Qt::Checked );
}

/******************************************************************************
    CheckDirTree::getMusicLibraryPath
******************************************************************************/
bool CheckDirTree::getMusicLibraryPath( QString& musicPath )
{
    bool ret = false;
    #ifdef WIN32
        if ( ( QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based ) == 0 )
        {
            // Use this for non-DOS-based Windowses
            
            // if it is vista:
            /*if( ( QSysInfo::WindowsVersion & QSysInfo::WV_VISTA ) )
            {
                //TODO: Test this.
                PWSTR ppszPath[MAX_PATH];
                HRESULT h =  SHGetKnownFolderPath( FOLDERID_Music, 0, 0, ppszPath );
                
                if ( h == S_OK )
                {
                    musicPath = QString::fromWCharArray( ppszPath );//QString::fromLocal8Bit( acPath );
                }
                else
                {
                    LOG( 1, "Couldn't get the path to the user's music folder! (Vista)\n" );
                    musicPath = QDir::homePath();
                }
            }
            else*/
            {
                TCHAR szPath[MAX_PATH];
                HRESULT h = SHGetFolderPath( NULL, CSIDL_MYMUSIC,
                                                NULL, 0, szPath );
                if ( h == S_OK )
                {
                    musicPath = QString::fromWCharArray( szPath );
                    ret = true;
                }
                else
                {
                    LOG( 1, "Couldn't get the path to the user's music folder!\n" );
                    musicPath = QDir::homePath();
                }
                //musicPath = QDir::homePath();
            }
        }
        else
        {
            LOG( 1, "This is a dos-based Windows System! Where is the music folder?" );
            musicPath = QDir::homePath();
        }
    #elif defined(Q_WS_MAC)
        QSettings ist( "apple.com", "iTunes" );
        QString path = ist.value( "AppleNavServices:ChooseObject:0:Path" ).toString();
        path = path.remove( "file://localhost" );
        qDebug() << "Found iTunes Library in:" << path;

        QFileInfo fi( path + "iTunes Music Library.xml" );
        if ( fi.exists() )
            musicPath = fi.absolutePath();
        else
            musicPath = QFileInfo( QDir::homePath() + "/Music/iTunes/iTunes Music Library.xml" ).absolutePath();
        
        ret = true;
    
    #elif defined(Q_WS_X11)
        musicPath = QDir::homePath();
    
    #else
        musicPath = QDir::homePath();
    #endif
    
    return ret;
}

/******************************************************************************
    CheckDirTree::check
******************************************************************************/
void
CheckDirTree::checkPath(
    QString path,
    Qt::CheckState state)
{
    QModelIndex index = m_dirModel.index(path);
    m_dirModel.setCheck(index, state);
    updateNode(index);
}

void
CheckDirTree::expandPath( QString path )
{
    QModelIndex index = m_dirModel.index(path);
    expand( index );
    updateNode( index );
}

void
CheckDirTree::showPath( QString path, QAbstractItemView::ScrollHint shint )
{
    QModelIndex index = m_dirModel.index(path);
    scrollTo( index, shint );
    updateNode( index );
}

/******************************************************************************
    CheckDirTree::setExclusions
******************************************************************************/
void
CheckDirTree::setExclusions(
    QStringList list)
{
    foreach(QString path, list)
    {
        checkPath(path, Qt::Unchecked);
    }
}

/******************************************************************************
    CheckDirTree::getExclusions
******************************************************************************/
QStringList
CheckDirTree::getExclusions()
{
    QStringList exclusions;
    QModelIndex root = rootIndex();

    getExclusionsForNode(root, exclusions);

    return exclusions;
}

/******************************************************************************
    CheckDirTree::getExclusionsForNode
******************************************************************************/
void
CheckDirTree::getExclusionsForNode(
    const QModelIndex& index,
    QStringList&       exclusions)
{
    // Look at first node
    // Is it checked?
    //  - move on to next node
    // Is it unchecked?
    //  - add to list
    //  - move to next node
    // Is it partially checked?
    //  - recurse

    int numChildren = m_dirModel.rowCount(index);
    for (int i = 0; i < numChildren; ++i)
    {
        QModelIndex kid = m_dirModel.index(i, 0, index);
        Qt::CheckState check = m_dirModel.getCheck(kid);
        if (check == Qt::Checked)
        {
            continue;
        }
        else if (check == Qt::Unchecked)
        {
            exclusions.append(m_dirModel.filePath(kid));
        }
        else if (check == Qt::PartiallyChecked)
        {
            getExclusionsForNode(kid, exclusions);
        }
        else
        {
            Q_ASSERT(false);
        }
    }
}

/******************************************************************************
    CheckDirTree::setInclusions
******************************************************************************/
void
CheckDirTree::setInclusions(
    QStringList list )
{
    foreach(QString path, list)
    {
        checkPath(path, Qt::Checked);
    }
}

/******************************************************************************
    CheckDirTree::getInclusions
******************************************************************************/
QStringList
CheckDirTree::getInclusions()
{
    QStringList inclusions;
    QModelIndex root = rootIndex();

    getInclusionsForNode(root, inclusions);

    return inclusions;
}

/******************************************************************************
    CheckDirTree::getInclusionsForNode
******************************************************************************/
void
CheckDirTree::getInclusionsForNode(
    const QModelIndex& index,
    QStringList&       inclusions )
{
    // Look at first node
    // Is it unchecked?
    //  - move on to next node
    // Is it checked?
    //  - add to list
    //  - move to next node
    // Is it partially checked?
    //  - recurse

    int numChildren = m_dirModel.rowCount(index);
    for (int i = 0; i < numChildren; ++i)
    {
        QModelIndex kid = m_dirModel.index(i, 0, index);
        Qt::CheckState check = m_dirModel.getCheck(kid);
        if (check == Qt::Checked)
        {
            inclusions.append(m_dirModel.filePath(kid));
        }
        else if (check == Qt::Unchecked)
        {
            continue;
        }
        else if (check == Qt::PartiallyChecked)
        {
            getInclusionsForNode(kid, inclusions);
        }
        else
        {
            Q_ASSERT(false);
        }
    }
}

/******************************************************************************
    CheckDirTree::onCollapse
******************************************************************************/
void
CheckDirTree::onCollapse(
    const QModelIndex& /*idx*/)
{

}

/******************************************************************************
    CheckDirTree::onExpand
******************************************************************************/
void
CheckDirTree::onExpand(
    const QModelIndex& idx)
{
    // If the node is partially checked, that means we have been below it
    // setting some stuff, so only fill down if we are unchecked.
    if (m_dirModel.getCheck(idx) != Qt::PartiallyChecked)
    {
        fillDown(idx);
    }
}

/******************************************************************************
    CheckDirTree::updateNode
    
    Updates the check state of visible nodes underneath the passed in node.    
******************************************************************************/
void
CheckDirTree::updateNode(
    const QModelIndex& idx)
{
    // Start by recursing down to the bottom and then work upwards
    fillDown(idx);
    updateParent(idx);
}

/******************************************************************************
    CheckDirTree::fillDown
    
    Takes a node index and propagates its state to all its child nodes.
******************************************************************************/
void
CheckDirTree::fillDown(
    const QModelIndex& parent)
{
    // Recursion stops when we reach a directory which has never been expanded
    // or one that has no children.
    if (!isExpanded(parent) ||
        !m_dirModel.hasChildren(parent))
    {
        return;
    }

    Qt::CheckState state = m_dirModel.getCheck(parent);
    int numChildren = m_dirModel.rowCount(parent);
    for (int i = 0; i < numChildren; ++i)
    {
        QModelIndex kid = m_dirModel.index(i, 0, parent);
        m_dirModel.setCheck(kid, state);
        fillDown(kid);
    }
}

/******************************************************************************
    CheckDirTree::updateParent
    
    Takes a node index and works out whether its parent is now checked,
    unchecked or partially checked. This is propagated up the tree.
******************************************************************************/
void
CheckDirTree::updateParent(
    const QModelIndex& index)
{
    QModelIndex parent = index.parent();
    
    if (!parent.isValid())
    {
        // We have reached the root
        return;
    }
    
    // Initialise overall state to state of first child
    QModelIndex kid = m_dirModel.index(0, 0, parent);
    Qt::CheckState overall = m_dirModel.getCheck(kid);

    int numChildren = m_dirModel.rowCount(parent);
    for (int i = 1; i < numChildren; ++i)
    {
        kid = m_dirModel.index(i, 0, parent);
        Qt::CheckState state = m_dirModel.getCheck(kid);
        if (state != overall)
        {
            // If we ever come across a state different than the first child,
            // we are partially checked
            overall = Qt::PartiallyChecked;
            break;
        }
    }
    
    m_dirModel.setCheck(parent, overall);

    updateParent(parent);
}    
