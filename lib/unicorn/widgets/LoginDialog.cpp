/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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
#include "lib/unicorn/LastMessageBox.h"
#include "lib/unicorn/Logger.h"
#include "lib/unicorn/UnicornUtils.h"
#include "lib/unicorn/ws/WsRequestBuilder.h"
#include "lib/unicorn/ws/WsReply.h"
#include <QMovie>
#include <QPushButton>


LoginDialog::LoginDialog()
           : m_bootstrap( false )
{
    ui.setupUi( this );
    ui.spinner->setMovie( new QMovie( ":/spinner.mng" ) );
    ui.spinner->movie()->setParent( this );
    ui.spinner->hide();
    
    ui.urls->setAttribute( Qt::WA_MacSmallSize );

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
    setWindowTitle( tr("Verifying Login Credentials...") );
    ok()->setEnabled( false );
    ui.spinner->show();
    ui.spinner->movie()->start(); //TODO spinner widget, integrate with QDesigner, stop and start on hide/show

    m_username = ui.username->text();

    using Unicorn::md5;
    QString const password = ui.password->text();

    WsReply* reply = WsRequestBuilder( "auth.getMobileSession" )
            .add( "username", m_username )
            .add( "authToken", md5( (m_username + md5( password.toUtf8() )).toUtf8() ) )
            .get();

    connect( reply, SIGNAL(finished( WsReply* )), SLOT(onAuthenticated( WsReply* )) );
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
                m_sessionKey = reply->lfm()["session"]["key"].nonEmptyText();
                accept();
                break;
            }
            catch (EasyDomElement::Exception& e)
            {
                qWarning() << e;
            }

            // FALL THROUGH!
        }
        
        case Ws::AuthenticationFailed:
            // COPYTODO
            LastMessageBox::critical( 
                    tr( "Login Failed" ), 
                    tr( "Sorry, we don't recognise that username, or you typed the password wrongly." ) );
            break;
        
        default:
            // COPYTODO
            LastMessageBox::critical(
                    tr("Last.fm Unavailable"), 
                    tr("There was a problem communicating with the Last.fm services. Please try again later.") );
            break;

        case Ws::UrProxyIsFuckedLol:
        case Ws::UrLocalNetworkIsFuckedLol:
            // TODO proxy prompting?
            // COPYTODO
            LastMessageBox::critical(
                    tr("Cannot connect to Last.fm"),
                    tr("Last.fm cannot be reached. Please check your firewall settings.") );
            break;
    }
    
    // do last, otherwise it looks weird
    ui.retranslateUi( this ); //resets Window title
    ok()->setEnabled( true );
    ui.spinner->hide();
    ui.spinner->movie()->stop();
}
