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

#include "PickDirsDialog.h"
#include <QtGui>

#define kBlurb "Boffin is a Last.fm technology that will generate a radio station using your local music collection."


PickDirsDialog::PickDirsDialog( QWidget* parent )
              : QDialog( parent, Qt::Sheet )
{
    ui.group = new QGroupBox( tr("My music is here:") );
    ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    ui.add = new QPushButton( tr("Add") );
    ui.add->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed );

    QLabel* blurb;

    QVBoxLayout* v = new QVBoxLayout( ui.group );
    v->addStretch();
    v->addSpacing( 10 );
    v->addWidget( ui.add );
    v->setAlignment( ui.add, Qt::AlignRight );
    v->setMargin( 10 );

    v = new QVBoxLayout( this );
    v->addWidget( blurb = new QLabel( kBlurb ) );
    v->addSpacing( 12 );
    v->addWidget( ui.group );
    v->addSpacing( 18 );
    v->addWidget( ui.buttons );
    v->setSpacing( 3 );

    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );

    connect( ui.add, SIGNAL(clicked()), SLOT(prompt()) );
    
    blurb->setWordWrap( true );
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Prepare Boffin") );

//////
    setMinimumWidth( 400 );

#ifdef Q_OS_MAC
    ui.group->setTitle( "" );
    QLabel* label;
    v->insertWidget( 2, label = new QLabel( "<b>Folders Containing Music") );
    label->setAttribute( Qt::WA_MacSmallSize );

    add( QDir::home().filePath( "Music" ) );
#endif
#ifdef Q_OS_WIN
    //TODO
#endif
#ifdef Q_WS_X11
    add( QDir::homePath() );
#endif
}


void
PickDirsDialog::add( const QString& path )
{
    if (path.isEmpty()) return;
    
    QToolButton* remove;
    QWidget* row = new QLabel( path );
    QHBoxLayout* h = new QHBoxLayout( row );
    h->setMargin( 0 );
    h->addStretch();
    h->addWidget( remove = new QToolButton );
    remove->setText( QChar(0x00002212) ); // the minus sign
    int const W = remove->sizeHint().width() - 7;
    remove->setFixedSize( W + 4, W );
    row->setFixedHeight( W );
    remove->show();
    connect( remove, SIGNAL(clicked()), row, SLOT(deleteLater()) );
    connect( row, SIGNAL(destroyed()), this, SLOT(enableDisableOk()) );
    
    QVBoxLayout* v = (QVBoxLayout*)ui.group->layout();
    v->insertWidget( v->count() - 3, row );
    
    enableDisableOk();
}


void
PickDirsDialog::prompt()
{
    add( QFileDialog::getExistingDirectory( this ) );
}


QStringList
PickDirsDialog::dirs() const
{
    QStringList dirs;
    foreach (QLabel* l, ui.group->findChildren<QLabel*>())
        dirs += l->text();
    dirs.removeAll( "" );
    return dirs;
}


void
PickDirsDialog::enableDisableOk()
{
    qDebug() << dirs();
    
    ui.buttons->button( QDialogButtonBox::Ok )->setEnabled( dirs().count() );
}
