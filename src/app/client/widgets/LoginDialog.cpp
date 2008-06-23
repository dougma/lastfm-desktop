/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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
#include "lib/unicorn/ws/VerifyUserRequest.h"
#include <QMovie>
#include <QPushButton>


LoginDialog::LoginDialog()
           : m_bootstrap( false )
{
    ui.setupUi( this );
    ui.spinner->setMovie( new QMovie( ":/spinner.mng" ) );
    ui.spinner->movie()->setParent( this );
    ui.spinner->hide();

    ok()->setDisabled( true );

    connect( ui.buttonBox, SIGNAL(accepted()), SLOT(verify()) );
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
LoginDialog::verify()
{
    setWindowTitle( tr("Verifying Login Credentials...") );
    ok()->setEnabled( false );
    ui.spinner->show();
    ui.spinner->movie()->start(); //TODO spinner widget, integrate with QDesigner, stop and start on hide/show

    VerifyUserRequest *verify = new VerifyUserRequest;
    verify->setUsername( ui.username->text() );
    verify->setPassword( ui.password->text() );
    
    connect( verify, SIGNAL(result( Request* )), SLOT(onVerifyResult( Request* )), Qt::QueuedConnection );

    verify->start();
}


void
LoginDialog::onVerifyResult( Request* request )
{
    ui.retranslateUi( this ); //resets Window title;
    ok()->setEnabled( true );
    ui.spinner->hide();
    ui.spinner->movie()->stop();

    VerifyUserRequest* verify = static_cast<VerifyUserRequest*>(request);

    // If the request failed, the auth code doesn't get filled in properly
    // since the ws refactor, so we need to check for it here.
    UserAuthCode result = verify->failed() ? AUTH_ERROR : verify->userAuthCode();

    LOGL( 4, "Verify result: " << (int)result );

    m_bootstrap = verify->bootstrapAllowed();

    switch (result)
    {
        case AUTH_OK:
        case AUTH_OK_LOWER:
            m_username = verify->username();
            m_password = verify->password();
            accept();
            break;

        case AUTH_BADUSER:
            LastMessageBox::critical( 
                    tr( "Login Failed" ), 
                    tr( "<p>That username was not found."
                        "<p>Please enter the username you used when you signed up at Last.fm." ) );
            break;
        
        case AUTH_BADPASS:
            LastMessageBox::critical( 
                    tr( "Login Failed" ), 
                    tr( "<p>The password you entered is not correct."
                        "<p>Please enter the password you used when you signed up at Last.fm." ) );
            break;
        
        case AUTH_ERROR:
            //TODO much better handling thatn v1
            break;
    }
}
