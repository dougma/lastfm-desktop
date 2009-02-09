/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#include "RestStateWidget.h"
#include "RestStateMessage.h"
#include "UglySettings.h"
#include <QKeyEvent>
#include <QDesktopServices>
#include <QPainter>

// These are pixel sizes
#ifdef WIN32
    // Small config
    static const int k_helloFontSize = 18;
    static const int k_standardFontSize = 11;
#else
    static const int k_helloFontSize = 24;
    static const int k_standardFontSize = 18;
#endif


static void setFontPixelSize( QWidget* widget, int const size )
{
    QFont f = widget->font();
    f.setPixelSize( size );
    widget->setFont( f );
}


RestStateWidget::RestStateWidget( QWidget* parent )
        : WatermarkWidget( parent )
        , m_play_enabled( true )
{
    ui.setupUi( this );

    setFontPixelSize( ui.hello, k_helloFontSize );
    setFontPixelSize( ui.label1, k_standardFontSize );
    setFontPixelSize( ui.label2, k_standardFontSize );

    #ifdef Q_WS_MAC
        QSize s = QPixmap(":/mac/RestStateWidgetCombo.png").size();
        ui.label2->setText( tr("or listen to your music in %1.", "%1 is a media player").arg( "iTunes" ) );

    #elif defined WIN32

        updatePlayerNames();

        // the long line is too wide and makes this->sizeHint huge unless we wordwrap
        ui.label2->setWordWrap( true );

        // Dirty hack to get heights looking the same on Windows
        ui.combo->setFixedHeight( 20 );
        ui.edit->setFixedHeight( 20 );
        ui.play->setFixedHeight( 22 );

    #elif defined LINUX

        ui.label2->hide(); //no plugins on Linux

        QList<QWidget*> widgets; widgets << ui.edit << ui.combo << ui.play;
        int h = 0;
        foreach ( QWidget* w, widgets )
            h = qMax( w->height(), h );
        foreach ( QWidget* w, widgets )
            w->setFixedHeight( h );

    #endif

    connect( ui.edit, SIGNAL( returnPressed() ), ui.play, SLOT( animateClick() ) );
    connect( ui.edit, SIGNAL( textChanged( QString ) ), SLOT( onEditTextChanged( QString ) ) );
    connect( ui.play, SIGNAL( clicked() ), SLOT( onPlayClicked() ) );

    ui.hello->setText( tr("Hello %1,").arg( The::currentUser().username() ) );
    ui.combo->setCurrentIndex( CurrentUserSettings().value( "RestStateWidgetComboIndex", 0 ).toInt() );

    setFocusProxy( ui.edit );

    ui.edit->installEventFilter( this );
}


RestStateWidget::~RestStateWidget()
{
    CurrentUserSettings().setValue( "RestStateWidgetComboIndex", ui.combo->currentIndex() );
}


void
RestStateWidget::updatePlayerNames()
{
#ifdef WIN32
    QStringList plugins = The::settings().allPlugins( false );
    plugins.removeAll( "" );

    if ( plugins.count() )
    {
        QString last_plugin = plugins.takeLast();

        if ( plugins.count() )
        {
            QString text = tr( "or listen to your music in %1 or %2.",
                    "%1 is a list of plugins, eg. 'Foobar, Winamp, Windows Media Player'" );

            ui.label2->setText( text.arg( plugins.join( ", " ) ).arg( last_plugin ) );
        }
        else
            ui.label2->setText( tr( "or listen to your music in %1.", "%1 is a media player" ).arg( last_plugin ) );
    }
    else
        ui.label2->setText( tr( "or install a player plugin to scrobble music from your media player." ) );
#endif
}


void
RestStateWidget::onHandshaken( Handshake* handshake )
{
    Q_UNUSED( handshake )
#if 0 //TODO
    #ifndef Q_WS_MAC
        const bool noPluginsInstalled = The::settings().allPlugins().empty();
    #else
        //There's always a Mac itunes "plugin"
        const bool noPluginsInstalled = false;
    #endif

    // HACK: Checking for isFirstRun prevents the little bootstrap prompt from appearing
    // if we're in the config wizard.
    if ( noPluginsInstalled || !handshake->isBootstrapPermitted() || The::settings().isFirstRun() )
        return;

    // FIXME: this should be moved elsewhere as it isn't really anything to do with
    // the RestState.
    if ( QFile::exists( MooseUtils::savePath( The::currentUsername() + "_" +
                                             The::currentUser().bootStrapPluginId() +
                                             "_bootstrap.xml" ) ) )
    {
        //Bootstrapping XML is ready to be submitted to the server
        The::app().onBootstrapReady( The::currentUsername(), The::currentUser().bootStrapPluginId() );
    }

    if ( !The::currentUser().bootStrapPluginId().isEmpty() )
        //A bootstrap must have been initiated so don't display bootstrapping message
        return;

    RestStateMessage* msg = new RestStateMessage( this );
    msg->setAcceptText( tr( "Do It!" ) );
    msg->setMessage( tr( "Do you want to import your media player listening history?\n"
        "This will add charts to your profile based on what you've listened to in the past." ) );
    msg->show();

    connect( msg, SIGNAL( accepted() ), SLOT( showBootstrapWizard()) );
    connect( msg, SIGNAL( moreHelpClicked() ), SLOT( openBootstrapFaq()) );
#endif
}


void
RestStateWidget::onEditTextChanged( const QString& new_text )
{
    ui.play->setEnabled( !new_text.isEmpty() && m_play_enabled );
}


void
RestStateWidget::setPlayEnabled( bool b )
{
    m_play_enabled = b;
    if ( !b )
        ui.play->setEnabled( false );
    else
        ui.play->setEnabled( !ui.edit->text().isEmpty() );
}


void
RestStateWidget::onPlayClicked()
{
    QString url;

    if ( ui.edit->text().startsWith( "lastfm://" ) )
        url = QString( "%1" ).arg( ui.edit->text() );

    else if ( ui.edit->text().startsWith ( "http://" ) )
        url = "";

    else if ( ui.combo->currentIndex() == 0 )
    {
        QString artist = ui.edit->text();
        artist = artist.trimmed();
        url = QString( "lastfm://artist/%1/similarartists" ).arg( UnicornUtils::urlEncodeItem( artist ) );
    }   
    else
    {
        QString tag = ui.edit->text();
        tag = tag.trimmed();
        url = QString( "lastfm://globaltags/%1" ).arg( UnicornUtils::urlEncodeItem( tag ) );
    }
    
    qDebug() << url;

    emit play( url );
}


void
RestStateWidget::clear()
{
    ui.edit->clear();
}


bool
RestStateWidget::eventFilter( QObject* o, QEvent* e )
{
    if ( o == ui.edit && e->type() == QEvent::KeyPress )
    {
        int const key = static_cast<QKeyEvent*>(e)->key();
        if ( key == Qt::Key_Up || key == Qt::Key_Down )
            //send these to the combo as a convenience feature
            ui.combo->event( e );
    }

    #ifdef Q_WS_MAC
    // can only do with english as the others have different width texts
    if ( o == ui.combo && e->type() == QEvent::Paint && The::settings().appLanguage() == "en" )
    {
        QRect r = ui.combo->rect();
        QPainter p( ui.combo );

        p.drawPixmap( QPoint( 0, 2 ), QPixmap( ":/mac/RestStateWidgetCombo.png" ) );
        r.adjust( 0, 0, -20, 0 );
        p.drawText( r, ui.combo->currentText(), Qt::AlignVCenter | Qt::AlignRight );
        return true;
    }
    #endif

    return false;
}

void
RestStateWidget::showBootstrapWizard()
{
    #if 0
    ConfigWizard d( this, ConfigWizard::BootStrap );

    if ( d.exec() == QDialog::Accepted )
        sender()->deleteLater();
    #endif
}


void
RestStateWidget::openBootstrapFaq()
{
    // TODO: might wanna link to the actual bootstrapping category in the faq
    // disabled for now as we don't have wanna hack on localized urls just one day before release
    QDesktopServices::openUrl(
        QUrl( "http://" + UnicornUtils::localizedHostName( The::settings().appLanguage() ) + "/help/faq/" ) ); // ?category=Listening History Importing" ) );
}
