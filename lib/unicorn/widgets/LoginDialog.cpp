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

#include "LoginDialog.h"
#include "common/qt/md5.cpp"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/lastfm/ws/WsRequestBuilder.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QtGui>
#ifdef WIN32
#include <windows.h>
#endif


LoginDialog::LoginDialog( const QString& username )
           : m_username( username )
           , m_subscriber( true )
{
    ui.setupUi( this );
    if (username.size())
    {
        ui.username->setText( username );
        ui.password->setFocus();
    }
	
#ifdef Q_WS_MAC
	ui.spacerItem->changeSize( 0, 0 );
	ui.spinner->hide();
	
	QVBoxLayout* v = new QVBoxLayout( ui.transient = new QDialog( this, Qt::Sheet ) );
	v->addWidget( ui.text = new QLabel( tr("Authenticating") ) );
	v->addWidget( ui.progress = new QProgressBar );
	v->addWidget( ui.cancel = new QPushButton( ' ' + tr("Cancel") + ' ' ) );
	v->setAlignment( ui.cancel, Qt::AlignRight );
	v->setSizeConstraint( QLayout::SetFixedSize );
	ui.text->setWordWrap( true );
	ui.cancel->setMinimumWidth( ui.cancel->sizeHint().width() );
	ui.transient->setModal( true );
	ui.progress->setRange( 0, 0 );
	ui.text->setFixedWidth( ui.text->sizeHint().width() * 2.5 );
    ui.urls->setAttribute( Qt::WA_MacSmallSize );
    
	connect( ui.cancel, SIGNAL(clicked()), ui.transient, SLOT(reject()) );
	connect( ui.transient, SIGNAL(rejected()), SLOT(cancel()) );

	//Qt is shit
	ui.buttonBox->layout()->setMargin( 0 );
	ui.buttonBox->setContentsMargins( 0, 0, -5, -7 );
	ui.buttonBox->layout()->setContentsMargins( 0, 0, 0, 0 );
	int left, top, right, bottom;
	layout()->getContentsMargins ( &left, &top, &right, &bottom );
	layout()->setContentsMargins( left, top, right, bottom - 10 );
	//Qt is shit
#else
    ui.spinner->hide();
#endif

	ok()->setText( tr("Log In") );
    ok()->setDisabled( true );

    connect( ui.buttonBox, SIGNAL(accepted()), SLOT(authenticate()) );
    connect( ui.buttonBox, SIGNAL(rejected()), SLOT(reject()) );
    connect( ui.username, SIGNAL(textEdited( QString )), SLOT(onEdited()) );
    connect( ui.password, SIGNAL(textEdited( QString )), SLOT(onEdited()) );
}


void
LoginDialog::onEdited()
{
    if (!ui.spinner->isVisible())
        ok()->setDisabled( ui.username->text().isEmpty() || ui.password->text().isEmpty() );
}


void
LoginDialog::authenticate()
{
    m_username = ui.username->text();
    m_password = Qt::md5( ui.password->text().toUtf8() );

    WsReply* reply = WsRequestBuilder( "auth.getMobileSession" )
            .add( "username", m_username )
            // always lowercase the username before generating the md5
            .add( "authToken", Qt::md5( (m_username + m_password).toUtf8() ) )
            .get();
	reply->setParent( this );

    connect( reply, SIGNAL(finished( WsReply* )), SLOT(onAuthenticated( WsReply* )) );
	
#ifdef Q_OS_MAC
	ui.transient->show();
#else
    ui.spinner->show();
    setWindowTitle( tr("Verifying Login Credentials...") );
    ok()->setEnabled( false );
#endif
}


void
LoginDialog::cancel()
{
	qDeleteAll( findChildren<WsReply*>() );

#ifdef Q_WS_MAC
	ui.transient->hide();
#endif
}


void
LoginDialog::onAuthenticated( WsReply* reply )
{
	switch (reply->error())
    {
        case Ws::NoError:
        {
            try
            {
                WsDomElement session = reply->lfm()["session"];
                
                // replace username; because eg. perhaps the user typed their
                // username with the wrong camel case
                QString username = session.optional( "name" ).text();
                if (username.size())
                    m_username = username;
                
                m_sessionKey = session["key"].nonEmptyText();
                m_subscriber = session["subscriber"].text() != "0";
                accept();
				
			#ifdef Q_WS_MAC
				ui.text->setText( "<b>Authentication successful" );
			#endif
                break;
            }
            catch (std::runtime_error& e)
            {
                qWarning() << e.what();
            }
			
            // FALL THROUGH!
        }
			
        case Ws::AuthenticationFailed:
            // COPYTODO
            QMessageBoxBuilder( this )
					.setIcon( QMessageBox::Critical )
					.setTitle( tr("Login Failed") )
					.setText( tr("Sorry, we don't recognise that username, or you typed the password wrongly.") )
					.exec();
            break;
			
        default:
            // COPYTODO
            QMessageBoxBuilder( this )
					.setIcon( QMessageBox::Critical )
					.setTitle( tr("Last.fm Unavailable") )
					.setText( tr("There was a problem communicating with the Last.fm services. Please try again later.") )
					.exec();
            break;
			
        case Ws::UrProxyIsFuckedLol:
        case Ws::UrLocalNetworkIsFuckedLol:
            // TODO proxy prompting?
            // COPYTODO
            QMessageBoxBuilder( this )
					.setIcon( QMessageBox::Critical )
					.setTitle( tr("Cannot connect to Last.fm") )
					.setText( tr("Last.fm cannot be reached. Please check your firewall or proxy settings.") )
					.exec();
        #ifdef WIN32
            // show Internet Settings Control Panel
            HMODULE h = LoadLibraryA( "InetCpl.cpl" );
            if (!h) break;
			BOOL (WINAPI *cpl)(HWND) = (BOOL (WINAPI *)(HWND)) GetProcAddress( h, "LaunchConnectionDialog" );
            if (cpl) cpl( winId() );
            FreeLibrary( h );
        #endif
            break;
    }
    
    
#ifdef Q_WS_MAC
	ui.transient->hide();
#else
    // do last, otherwise it looks weird
    ui.retranslateUi( this ); //resets Window title
    ok()->setEnabled( true );
    ui.spinner->hide();
#endif
}
