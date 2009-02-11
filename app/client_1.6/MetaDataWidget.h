#ifndef METADATAWIDGET_H
#define METADATAWIDGET_H

#include <QWebView>

class MetaDataWidget : public QWebView
{
Q_OBJECT
public:
    MetaDataWidget( QWidget* parent = 0 );
    
    void setTuningIn();
    void setTrack( const Track& );

protected slots:
    void onInfoFetched( WsReply* );

protected:
    Track m_track;
};

#endif //METADATAWIDGET_H
