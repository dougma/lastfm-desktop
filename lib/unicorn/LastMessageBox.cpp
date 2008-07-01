/***************************************************************************
*   Copyright (C) 2007 by                                                 *
*      Philipp Maihart, Last.fm Ltd <phil@last.fm>                        *
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

#include <QApplication>
#include <QPushButton>
#include <QDialogButtonBox>

#include "LastMessageBox.h"
#include "Logger.h"

// for DRY reasons
#define PARENT parent ? parent : mainWindow()


static QWidget*
mainWindow()
{
    QList<QWidget*> widgets = qApp->topLevelWidgets();

    foreach (QWidget* w, widgets)
        if (w->objectName() == "MainWindow")
            return w;

    foreach (QWidget* w, widgets)
        if (w->isVisible())
            // on windows invisible windows don't parent dialogs as intended
            return w;

    return 0;
}


LastMessageBox::LastMessageBox( QWidget* parent )
        : QMessageBox( PARENT )
{}


#ifdef Q_OS_MAC
/** this function prevents a bug in Qt where showing a sheet parented to the 
  * container blocks the application if another modal dialog is visible */
static Qt::WindowFlags
macFlags( Qt::WindowFlags f )
{
    int x = 0;
    foreach (QWidget* w, qApp->topLevelWidgets())
        if (w->isVisible())
            x++;

    if (x < 2)
        f |= Qt::Sheet;
        
    return f & (~Qt::Drawer); // we remove Drawer due to bug where this was appearing!
}
#endif


LastMessageBox::LastMessageBox( Icon icon,
                                const QString& title,
                                const QString& text,
                                StandardButtons buttons,
                                QWidget* parent,
                                Qt::WindowFlags f,
                                QStringList buttonTexts )
#ifdef Q_WS_MAC
        : QMessageBox( icon, title, title + "\t\t\t", buttons, PARENT, macFlags( f ) )
#else
        : QMessageBox( icon, title, text, buttons, PARENT, f )
#endif
{
    #ifdef Q_WS_MAC
        setInformativeText( text );

        if ( buttons == QMessageBox::Yes )
            buttons = QMessageBox::Yes | QMessageBox::No;

        uint mask = QMessageBox::FirstButton;
        int buttonTextIndex = 0;

        if ( !buttonTexts.isEmpty() )
        {
            while (mask <= QMessageBox::LastButton) 
            {
                uint sb = buttons & mask;
                mask <<= 1;
                if (!sb)
                    continue;

                QAbstractButton *button = QMessageBox::button((QMessageBox::StandardButton)sb);

                if ( buttonTexts.count() > buttonTextIndex ) 
                {
                    button->setText( buttonTexts.at( buttonTextIndex ) );
                    buttonTextIndex++;
                }
            }
        }
    #else
        Q_UNUSED( title );
        Q_UNUSED( buttonTexts );
    #endif
}


void
LastMessageBox::setText( const QString & text )
{
    #ifdef Q_WS_MAC
        QMessageBox::setInformativeText( text );
    #else
        QMessageBox::setText( text );
    #endif
}


void
LastMessageBox::setWindowTitle( const QString & title )
{
    #ifdef Q_WS_MAC
        QMessageBox::setText( title + "\t\t\t" );
    #else
        QMessageBox::setWindowTitle( title );
    #endif
}


QSize //virtual
LastMessageBox::sizeHint() const
{
    QSize hintSize = QMessageBox::sizeHint();
    QSize minSize( 480, 153 );

    if ( hintSize.width() < minSize.width() )
    {
        hintSize.setWidth( minSize.width() );
    }
    else if ( hintSize.height() < minSize.height() )
    {
        hintSize.setHeight( minSize.height() );
    }

    return hintSize;
}


QMessageBox::StandardButton
LastMessageBox::critical( const QString& title,
                          const QString& text,
                          StandardButtons buttons,
                          StandardButton defaultButton,
                          QStringList buttonTexts,
                          QWidget* parent )

{
    QApplication::setOverrideCursor( Qt::ArrowCursor );

    #ifdef Q_WS_MAC
        QMessageBox::StandardButton result = showMacMessageBox( PARENT, QMessageBox::Critical, title, text, buttons, defaultButton, buttonTexts );
    #else
        Q_UNUSED( title );
        Q_UNUSED( buttonTexts );
        QApplication::restoreOverrideCursor();
        QMessageBox::StandardButton result = QMessageBox::critical( PARENT, title, text, buttons, defaultButton );
    #endif
    return result;
}


QMessageBox::StandardButton
LastMessageBox::information( const QString& title,
                             const QString& text,
                             StandardButtons buttons,
                             StandardButton defaultButton,
                             QStringList buttonTexts,
                             QWidget* parent )

{
    QApplication::setOverrideCursor( Qt::ArrowCursor );

    #ifdef Q_WS_MAC
        QMessageBox::StandardButton result = showMacMessageBox( PARENT, QMessageBox::Information, title, text, buttons, defaultButton, buttonTexts );
    #else
        Q_UNUSED( title );
        Q_UNUSED( buttonTexts );
        QMessageBox::StandardButton result = QMessageBox::information( PARENT, title, text, buttons, defaultButton );
    #endif

    QApplication::restoreOverrideCursor();
    return result;
}


QMessageBox::StandardButton
LastMessageBox::question( const QString& title,
                          const QString& text,
                          StandardButtons buttons,
                          StandardButton defaultButton,
                          QStringList buttonTexts,
                          QWidget* parent )
{
    QApplication::setOverrideCursor( Qt::ArrowCursor );

    #ifdef Q_WS_MAC
        QMessageBox::StandardButton result = showMacMessageBox( PARENT, QMessageBox::Question, title, text, buttons, defaultButton, buttonTexts );
    #else
        Q_UNUSED( title );
        Q_UNUSED( buttonTexts );
        QMessageBox::StandardButton result = QMessageBox::question( PARENT, title, text, buttons, defaultButton );
    #endif

    QApplication::restoreOverrideCursor();
    return result;
}


QMessageBox::StandardButton 
LastMessageBox::warning( const QString& title,
                         const QString& text,
                         StandardButtons buttons,
                         StandardButton defaultButton,
                         QStringList buttonTexts,
                         QWidget* parent )
{
    QApplication::setOverrideCursor( Qt::ArrowCursor );

    #ifdef Q_WS_MAC
        QMessageBox::StandardButton result = showMacMessageBox( PARENT, QMessageBox::Warning, title, text, buttons, defaultButton, buttonTexts );
    #else
        Q_UNUSED( title );
        Q_UNUSED( buttonTexts );
        QMessageBox::StandardButton result = QMessageBox::warning( PARENT, title, text, buttons, defaultButton );
    #endif

    QApplication::restoreOverrideCursor();

    return result;
}


#ifdef Q_WS_MAC
QMessageBox::StandardButton 
showMacMessageBox( QWidget *parent,
                   QMessageBox::Icon icon,
                   const QString& title,
                   const QString& text,
                   QMessageBox::StandardButtons buttons,
                   QMessageBox::StandardButton defaultButton,
                   QStringList buttonTexts )
{
    QMessageBox msgBox(icon, title, title + "\t\t\t", QMessageBox::NoButton, parent, macFlags( 0 ) );
    msgBox.setInformativeText( text );

    QDialogButtonBox *buttonBox = qFindChild<QDialogButtonBox*>(&msgBox);
    Q_ASSERT( buttonBox != 0 );

    uint mask = QMessageBox::FirstButton;
    int buttonTextIndex = 0;

    if ( buttons == QMessageBox::Yes )
        buttons = QMessageBox::Yes | QMessageBox::No;

    while (mask <= QMessageBox::LastButton) 
    {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb)
            continue;
        QPushButton *button = msgBox.addButton( (QMessageBox::StandardButton)sb );

        if ( !buttonTexts.isEmpty() )
        {
            if ( buttonTexts.count() > buttonTextIndex ) 
            {
                button->setText( buttonTexts.at( buttonTextIndex ) );
                buttonTextIndex++;
            }
        }

        // Choose the first accept role as the default
        if (msgBox.defaultButton())
            continue;

        if ((defaultButton == QMessageBox::NoButton && buttonBox->buttonRole( (QAbstractButton*) button) == QDialogButtonBox::AcceptRole)
            || (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }

    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;

    return msgBox.standardButton(msgBox.clickedButton());
}
#endif
