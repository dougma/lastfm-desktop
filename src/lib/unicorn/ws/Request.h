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

#ifndef WEB_SERVICE_REQUEST_H
#define WEB_SERVICE_REQUEST_H

#include "common/DllExportMacro.h"

#include "UnicornCommon.h"

#include <QHttp>
#include <QHttpHeader>
#include <QTimer>

//TODO mxcl check error handling, since that was the point of all this
//TODO escape query paramaeters in paths in get function
//TODO intercept the response header, error on anything but status code 200
//TODO do all things need to be url percent encoded for get?
//TODO metadata timeout stuff used to have a progressively increasing timer


// Some nice macros for writing getters and setters automatically
#define PROP_GET( Type, lower ) \
    private: Type m_##lower; \
    public: Type lower() const { return m_##lower; } \
    private:
#define PROP_GET_SET( Type, lower, Upper ) \
    PROP_GET( Type, lower ) \
    public: void set##Upper( Type lower ) { m_##lower = lower; } \
    private:


enum RequestType
{
    TypeHandshake,
    TypeChangeStation,
    TypeGetXspfPlaylist,
    TypeSetTag,
    TypeWebService,
    TypeSkip,
    TypeArtistMetaData,
    TypeTrackMetaData,
    TypeVerifyUser,
    TypeEnableScrobbling,
    TypeEnableDiscoveryMode,
    TypeAddToMyPlaylist,
    TypeLove,
    TypeUnLove,
    TypeBan,
    TypeUnBan,
    TypeFriends,
    TypeUserPictures,
    TypeRecentTracks,
    TypeRecentlyBannedTracks,
    TypeRecentlyLovedTracks,
    TypeNeighbours,
    TypeSimilarArtists,
    TypeDeleteFriend,
    TypeRecommend,
    TypeUserPicturesRequest,
    TypeReportRebuffering,
    TypeSubmitFingerprint,
    TypeFingerprintQuery,
    TypeFrikkinNorman,

    TypeUserTags,
    TypeUserArtistTags,
    TypeUserAlbumTags,
    TypeUserTrackTags,
    TypeSimilarTags,
    TypeSearchTag,
    TypeArtistTags,
    TypeTrackTags,
    TypeAlbumTags,
    TypeTopTags,

    TypeUserLabels,
    TypeTrackUpload,

    TypeProxyTest
};


/**
* Global enum for all web request error codes. This is so that Request*
* object pointers can be passedup a hierarchy and still be switched on
* specific error code.
*
* Admittedly, this is shit as it forces you to update this enum every time
* you add a new request subclass and introduces ugly coupling between them,
* but I spent considerable time thinking about a way of having each subclass
* define its own error codes whilst still keeping them globally unique (even
* enlisting Norman's considerable template experience), but to no avail.
* Sometimes C++ really sucks. So global enum, here we go. :(
*/
enum WebRequestResultCode
{
    // class Request codes
    // ------------------------------------------------------------------------
    Request_Undefined,
    Request_Success,

    /// We aborted it, so the user prolly doesn't care
    Request_Aborted,

    /// DNS failed
    Request_HostNotFound,

    /// HTTP response code
    Request_BadResponseCode,

    /// We've timed out waiting for an HTTP response several times
    Request_NoResponse,

    /// Proxy authentication required, probably show proxy settings dialog
    Request_ProxyAuthenticationRequired,

    /// Authentication failed, probably show user settings dialog
    Request_WrongUserNameOrPassword,

    // class Handshake codes
    // ------------------------------------------------------------------------
    Handshake_WrongUserNameOrPassword,
    Handshake_Banned,
    Handshake_SessionFailed,

    // class ChangeStationRequest codes
    // ------------------------------------------------------------------------
    ChangeStation_NotEnoughContent,     // there is not enough content to play this station.
    ChangeStation_TooFewGroupMembers,   // this group does not have enough members for radio.
    ChangeStation_TooFewFans,           // this artist does not have enough fans for radio.
    ChangeStation_TooFewNeighbours,     // there are not enough neighbours for this radio.
    ChangeStation_Unavailable,          // this item is not available for streaming.
    ChangeStation_SubscribersOnly,      // this feature is only available to subscribers.
    ChangeStation_StreamerOffline,      // the streaming system is offline for maintenance
    ChangeStation_InvalidSession,       // session has timed out, please re-handshake
    ChangeStation_UnknownError,         // no idea

    // class GetXspfPlaylistRequest codes
    // ------------------------------------------------------------------------
    Playlist_InvalidSession,            // 401, session timed out, need to re-handshake
    Playlist_RecSysDown,                // 503, recommendation systems down, treat as connection error

    // class FingerprintQueryRequest codes
    // ------------------------------------------------------------------------
    Fingerprint_QueryError,             // no fpid was returned, just an error string

    // class <insert new class name here> codes
    // ------------------------------------------------------------------------


    /// Custom undefined error
    WebRequestResult_Custom = 1000
};


 /**
  * @author <max@last.fm>
  * @short Abstract base class for requests to the last.fm WebService
  *
  * rationale: free error handling for basic types, avoids some repeated code
  * encourages more thorough error handling through error( Type ) signal
  *
  * Reimplement Request, reimplement start() and success().
  * 
  * After success or failure The::webService() will alert the rest of the app
  * of the result. You can connect to that or to the instantiation result()
  * signal and process the data there.
  */

class DLLEXPORT Request : public QObject
{
    Q_OBJECT

public:
    virtual ~Request();

    RequestType type() const { return m_type; }

    WebRequestResultCode resultCode() const { return m_result; }

    /**
     * This function provides access to the HTTP response header status code
     * (200, 404, 503) etc should client code need to look at it.
     */
    int responseHeaderCode() const { return m_responseHeaderCode; }

    bool failed() const { return m_result != Request_Success; }
    bool succeeded() const { return m_result == Request_Success; }
    bool aborted() const { return m_result == Request_Aborted; }

    QString errorMessage() const;

    /// default is true
    void setAutoDelete( bool b ) { m_auto_delete = b; }
    bool autoDelete() const { return m_auto_delete; }

    // This needs to be called by the Handshake request
    static void setBaseHost( QString host ) { m_baseHost = host; }
    static QString baseHost() { return m_baseHost; }

    /// The application needs to call this at initialisation time.
    static void setLanguage( QString l ) { s_language = l; }
    static QString language() { return s_language; }

public slots:
    /**
     * reimplement and make a call to get() or request()
     * this function may be called multiple times, so do
     * initialisations, etc. in the ctor
     */
    virtual void start() = 0;

    /// result will be set to ResultAborted, but the abort is asynchronous
    void abort();

    /// tries again in increasing time intervals
    /// user gets a status message
    void tryAgain();

signals:
    /**
     * you can connect to this or to the generic WebService version
     */
    void result( Request* );

protected:
    /**
    * protected - this is an abstract base class
    * The::webService() owns the Request, and will handle deletion
    *
    * @p type add a value to the enum in fwd.h
    * @p name set to same as class name or something
    */
    Request( RequestType type, const char *name );

    /**
     * We successfully received a header from Last.fm.
     * If you're interested in specific header status codes, reimplement
     * this function in the subclass and return true to indicate you have
     * handled the header. This default implementaion returns false which
     * causes some default error handling of http header codes to take place.
     */
    virtual bool headerReceived( const class QHttpResponseHeader& )
    {
        return false;
    }

    /**
     * We successfully received data from Last.fm.
     * Reimplement and parse the data. After this function is called,
     * emit result( this ) will be called and then The::webService() will
     * receive this object, and it will emit The::webService()->result()
     * You may still call setFailed() inside your implementation to classify the
     * request as failed.
     */
    virtual void success( QByteArray /* data */ ) {}

    /// will do QHttp::setHost( @p path, @p port )
    void setHost( QString path, int port = 80 );

    /// will do QHttp::get( @p path )
    void get( QString path );

    void post( QString path, QByteArray& data );
    void post( QHttpRequestHeader& header, QByteArray& data );

    /// performs the XmlRpc request
    void request( const class XmlRpc& );

    /// calls to failed() will return true
    void setFailed( WebRequestResultCode type, QString message = "" )
    {
        m_result = type;
        if ( !message.isEmpty() )
        {
            m_error = message;
        }
    }

    void setOverrideCursor();

    /// archaic, don't use
    QString parameter( QString key, QString data );

    QHttp* http() { return m_http; } //FIXME: should we add this one?
    QTimer& timeoutTimer() { return m_timeout_timer; } //and this one?

private:
    PROP_GET( QByteArray, data )

    QHttp* m_http;

    QString m_error;

    WebRequestResultCode m_result;
    int m_responseHeaderCode;

    bool m_auto_delete;

    RequestType m_type;

    QTimer m_timeout_timer;
    QTimer m_retry_timer;

    bool m_override_cursor;

    static QString m_baseHost;

    static QString s_language;

private slots:
    void onHeaderReceivedPrivate( const class QHttpResponseHeader& header );
    void onFailurePrivate( int error_code, const QString &error_string );
    void onSuccessPrivate( QByteArray data );
};

#endif
