#include "MetaDataWidget.h"


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
    setHtml( t.toString() );
}
