
#include <lastfm/Track.h"
class TrackInfo : public Track
{
public:
    TrackInfo( const Track& t ) : Track( t )
    {}
    
    QString track() const { return title(); }
};
#define MetaData TrackInfo


#include "State>
#include <lastfm/WsError.h"
#include "RadioEnums>
#define RadioError Ws::Error


class WsReply;
#define Request WsReply


#include <lastfm/RadioStation.h"
#define Station RadioStation


#include "UnicornCommon>



namespace MooseEnums
{
    enum UserIconColour
    {
        eNone = -1,
        eRed = 0,
        eBlue,
        eGreen,
        eOrange,
        eBlack,
        eColorMax
    };
}


class UglyUser
{
    QString m_name;
    
public:
    bool isSubscriber() const;
    QString name() const { return m_name; }
};

class LastFmSettings;
class Container;
class Radio;
class ShareDialog;
namespace The
{
    Radio& radio();
    Container& container();
    LastFmSettings& settings();
    UglyUser& user();
    ShareDialog& shareDialog();
}


namespace MooseConstants
{
    // SCROBBLING CONSTANTS

    // The plugin ID used by HttpInput when submitting radio tracks to the player listener
    const QString kRadioPluginId = "radio";

    // Limits for user-configurable scrobble point (%)
    const int kScrobblePointMin = 50;
    const int kScrobblePointMax = 100;

    // Shortest track length allowed to scrobble (s)
    const int kScrobbleMinLength = 31;

    // Upper limit for scrobble time (s)
    const int kScrobbleTimeMax = 240;

    // Min size of buffer holding streamed http data, i.e the size the http
    // buffer needs to get to before we start streaming.
    const int kHttpBufferMinSize = 16 * 1024;

    // Max
    const int kHttpBufferMaxSize = 256 * 1024;

    const int kScrobblePoint = 50;
};
