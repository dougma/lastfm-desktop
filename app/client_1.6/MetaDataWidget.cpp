#include "MetaDataWidget.h"
#include <lastfm/WsReply>
#include <QPalette>

static const QString css = 
    "<style>"
    "img { "
    "    float: left;"
    "    display: inline;"
    "    padding-right: 2em;"
    "    padding-bottom: 1.5em;"
    "}"
    
    "body {"
    "    background-image: url('http://cdn.last.fm/client/images/watermark.png');"
    "    background-repeat: no-repeat;"
    "    background-position: right bottom;"
    "    background-attachment: fixed;"
    "}"
    "h1 {"
    "   font-size: 2em;"
    "   margin: 0em;"
    "   padding: 0em;"
    "}"
    "h2 {"
    "   font-size:1.5em;"
    "   margin: 0em;"
    "   padding: 0em;"
    "}"
    "</style>";

MetaDataWidget::MetaDataWidget( QWidget* parent )
              : QWebView( parent )
{
}

void
MetaDataWidget::setTuningIn()
{
    setHtml( "Tuning In..." );
}


void
MetaDataWidget::setTrack( const Track& t )
{
    WsReply* r = t.artist().getInfo();
    connect( r, SIGNAL( finished( WsReply* )), SLOT( onInfoFetched( WsReply* )));
    m_track = t;
}


void 
MetaDataWidget::onInfoFetched( WsReply* r )
{
    const QString bio = r->lfm()["artist"]["bio"]["content"].text();

    QStringList paras = bio.split( QRegExp( "\\r+" ));
    
    QString formattedBio;
    if( !paras.empty() )
    { 
        paras.replaceInStrings( QRegExp( "^(.*)$" ), "<p>\\1</p>" );
        formattedBio = paras.join( "" );    
    }
    
    const QString artistImageUrl = r->lfm()["artist"]["image size=large"].text();

    formattedBio.prepend( "<html><head>"+ css +"</head><body><img src='" + artistImageUrl + "' />" 
                          "<h1>" + m_track.title()  + "</h1>"
                          "<h2>" + tr( "by" ) + " " + r->lfm()["artist"]["name"].text() + "</h2>"
                        );
    formattedBio.append( "</body></html>" );
    setHtml( formattedBio );
}


