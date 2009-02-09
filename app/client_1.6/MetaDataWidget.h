
#include <QWebView>


class MetaDataWidget : public QWebView
{
public:
    MetaDataWidget( QWidget* parent = 0 );
    
    void setTuningIn();
    void setTrack( const class Track& );
};
