

#include "lib/unicorn/UnicornApplication.h"


class App : public Unicorn::Application
{
    Q_OBJECT
    
    class StateMachine* machine;
    class Radio* radio;
    class Scrobbler* scrobbler;
    class Container* container;
    class QSystemTrayIcon* trayicon;
    
public:
    App( int&, char** );
    ~App();

    void setContainer( class Container* );
    
public slots:
    void open( const QUrl& );
    void parseArguments( const QStringList& );
    void setScrobblingEnabled( bool );
    void logout();

signals:
    /** documented in PlayerManager */
    void playerChanged( const QString& name );
    void trackSpooled( const Track&, class StopWatch* = 0 );
    void trackUnspooled( const Track& );
    void stopped();
    void stateChanged( State newstate, const Track& = Track() ); //convenience
    void scrobblePointReached( const Track& );

private slots:
    /** all webservices connect to this and emit in the case of bad errors that
     * need to be handled at a higher level */
    void onWsError( Ws::Error );
    void onRadioError( int, const class QVariant& );
    
	void onScrobblerStatusChanged( int );
    void onTrackSpooled( const Track& );
    void onTrackUnspooled( const Track& );
    void onUserGotInfo( WsReply* );

};
