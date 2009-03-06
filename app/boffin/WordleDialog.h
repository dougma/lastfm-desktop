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

#ifndef WORDLE_DIALOG_H
#define WORDLE_DIALOG_H

#include <QTextEdit>
#include <QVBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QPushButton>


class WordleDialog : public QDialog
{
    QTextEdit* output;

public:
    WordleDialog( QWidget* parent = 0 ) : QDialog( parent )
    {
        new QVBoxLayout( this );
        
        layout()->addWidget( output = new QTextEdit());
        output->setReadOnly( true );

        QLabel* hint;        
        layout()->addWidget( hint = new QLabel);
        hint->setOpenExternalLinks( true );
        hint->setTextFormat( Qt::RichText );
        hint->setText( tr("<p>We copied the wordlist to your clipboard already.<br>"
                      "So just visit <a href='http://www.wordle.net/advanced'>Wordle Advanced</a> and paste!</p>"
                      "<p>Why not save your Wordle with username \"boffin\" and your<br>"
                      "Last.fm username as the title so we can start a <a href='http://www.wordle.net/gallery?username=Boffin'>community</a>?</p>") );
        
        QDialogButtonBox* buttons;
        layout()->addWidget( buttons = new QDialogButtonBox( QDialogButtonBox::Ok ));
        connect( buttons, SIGNAL(accepted()), SLOT( accept()));
        buttons->button( QDialogButtonBox::Ok )->setText( tr("Close") );
        
        setWindowTitle( tr("Your Wordlized Tags") );
    }

    void setText( const QString& t )
    {
        output->setText( t );
        //output->selectAll(); //looks wrong
        QApplication::clipboard()->setText( t );
    }
};

#endif //WORDLE_DIALOG_H
