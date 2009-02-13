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

#include <QDialog>

 
class PickDirsDialog : public QDialog
{
    Q_OBJECT

    struct Ui
    {
        class QDialogButtonBox* buttons;
        class QGroupBox* group;
        class QPushButton* add;
        
    } ui;
    
    void add( const QString& path );

private slots:
    void prompt();
    void enableDisableOk();

public:
    PickDirsDialog( QWidget* parent );
    
    QStringList getDirs() const;
    void setDirs(QStringList dirs);
};
